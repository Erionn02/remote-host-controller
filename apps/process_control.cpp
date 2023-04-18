#include <boost/asio/io_service.hpp>
#include <boost/process.hpp>
#include <boost/process/async.hpp>

#include <iostream>
#include <thread>
#include <oneapi/tbb/concurrent_queue.h>

namespace asio = boost::asio;
namespace bp = boost::process;
using namespace std::placeholders;

enum class StreamType {
    normal,
    error
};
using SingleOutput = std::pair<StreamType, std::string>;
using StreamOutputsQueue = tbb::concurrent_queue<SingleOutput>;

struct PipeReader {
    using error_code = boost::system::error_code;
    using stringbuf = asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>;

    bp::async_pipe pipe;
    std::string output;
    stringbuf buf = asio::dynamic_buffer(output);
    std::shared_ptr<StreamOutputsQueue> queue;
    StreamType stream_type;
    std::size_t buffer_size{2048};

    PipeReader(asio::io_service &ios, std::shared_ptr<StreamOutputsQueue>& queue, StreamType stream_type) : pipe(ios),
                                                                                          queue(queue),
                                                                                          stream_type(stream_type) {
        buf.prepare(buffer_size);
    }

    void read_loop(const error_code &ec = {}, size_t n = {}) {

        if (n > 0) {
            queue->emplace(stream_type, output.substr(0, n));
        }

        if (!ec)
            pipe.async_read_some(buf.data(0, buffer_size), [this](const error_code &ec, std::size_t n) {
                read_loop(ec, n);
            });
    }
};

int main() {
    asio::io_context ios;
    std::shared_ptr<StreamOutputsQueue> outputs = std::make_shared<StreamOutputsQueue>();
    PipeReader pipeOut(ios, outputs, StreamType::normal), pipeErr(ios, outputs, StreamType::error);
    bp::opstream in;

    bp::child c(
            "test_program",
            bp::std_out > pipeOut.pipe,
            bp::std_err > pipeErr.pipe,
            bp::std_in < in,
            ios);

    pipeOut.read_loop();
    pipeErr.read_loop();

    std::jthread io_thread([&ios] { ios.run(); });
    std::jthread print_thread([outputs, &c] { while(c.running()){
        SingleOutput output;
        if(outputs->try_pop(output)){
            if(output.first == StreamType::normal){
                std::cout<<output.second;
            } else {
                std::cerr<<output.second;
            }
        }
    } });

    std::cout << "STARTING LOOP: \n";
    for (std::string input_command{};
         std::getline(std::cin, input_command) && c.running();
         std::this_thread::yield()) //
    {
        in << input_command << std::endl;
    }

    std::cout << "\n---\ntotal stdout:" << pipeOut.output.length() << ", stderr:" << pipeErr.output.length()
              << std::endl;
    return 0;
}