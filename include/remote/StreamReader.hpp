#pragma once
#include "Utils.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <oneapi/tbb/concurrent_queue.h>

namespace asio = boost::asio;
namespace bp = boost::process;

struct StreamOutput {
    std::string stream_type{};
    std::string content{};
};

using StreamOutputQueue = tbb::concurrent_queue<StreamOutput>;

class StreamReader {
public:
    using error_code = boost::system::error_code;
    using StringBuffer = asio::dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>;

    StreamReader(asio::io_service &ios,StreamOutputQueue &queue, const std::string& stream_type);

    bp::async_pipe &getPipe();


private:
    void read_loop(const error_code &ec = {}, size_t read_size = {});


    bp::async_pipe pipe;
    std::string output;
    StringBuffer buffer = asio::dynamic_buffer(output);
    StreamOutputQueue& queue;
    std::string stream_type;
    std::size_t buffer_size{2048};
};


