#include "remote/StreamReader.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/process.hpp>

#include <iostream>
#include <thread>

namespace asio = boost::asio;
namespace bp = boost::process;
using namespace std::placeholders;

ZMQSocket sock{zmq::socket_type::push};

struct PipeReaderR {
    using error_code = boost::system::error_code;
    using stringbuf = asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>;

    bp::async_pipe pipe;
    std::string output;
    stringbuf buf = asio::dynamic_buffer(output);
    std::ostream &mirror;

    PipeReaderR(asio::io_service &ios, std::ostream &stream) : pipe(ios), mirror(stream) {}

    void read_loop(error_code ec = {}, size_t n = {}) {
//            std::cerr << "[fd:" << pipe.native_source() << " " << n << ", " << ec.message() << "]";
//        zmq::message_t m{std::string("EVENT")};
//        if(!sock.send(m)) exit(0);
        if (n) {
            buf.commit(n);
//            mirror << "\x1B[2J\x1B[H";
            mirror << std::string_view(output).substr(output.size() - n) << std::flush;
        }

        if (!ec){
//            output.clear();
            pipe.async_read_some(buf.prepare(10000), std::bind(&PipeReaderR::read_loop, this, _1, _2));
        }
    }
};

int main() {
//    sock.connect("tcp://127.0.0.1:2137");
    std::string error{};

    asio::io_context ios;

    PipeReaderR pipeOut(ios, std::cout), pipeErr(ios, std::cerr);
    bp::opstream in;

    bp::child c(                    //
            "test_program",     //
            bp::std_out > pipeOut.pipe, //
            bp::std_err > pipeErr.pipe, //
            bp::std_in < in,            //
            ios);

    pipeOut.read_loop();
    pipeErr.read_loop();
    std::jthread io_thread([&ios] { ios.run(); });

    std::cout << "STARTING LOOP: \n";

    while(c.running()){
//        if(lockf(in.rdbuf()->pipe().native_sink(), F_TEST, 0) == 0){
//        } else {
//            if (errno == EACCES || errno == EAGAIN) {
//                std::cout<<"ENTERING WAITING FOR INPUT: ";
                std::string input_command{};
                std::getline(std::cin, input_command);
                in << input_command << std::endl;
//            }
//        }
//        std::cout<<errno<<'\n';
//        std::cout<<"loop";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\n---\ntotal stdout:" << pipeOut.output.length() << ", stderr:" << pipeErr.output.length()
              << std::endl;
    return 0;
}
