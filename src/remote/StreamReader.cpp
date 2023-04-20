#include "remote/StreamReader.hpp"


StreamReader::StreamReader(asio::io_service &ios, StreamOutputQueue &queue, const std::string& stream_type)
        : pipe(ios),
          queue(queue),
          stream_type(stream_type) {
    buffer.prepare(buffer_size);
    pipe.async_read_some(buffer.data(0, buffer_size),
                         [this](const error_code &ec, std::size_t n) { read_loop(ec, n); });
}

bp::async_pipe &StreamReader::getPipe() {
    return pipe;
}

void StreamReader::read_loop(const error_code &ec, size_t read_size) {
    if (read_size > 0) {
        queue.emplace(StreamOutput{stream_type, output.substr(0, read_size)});
    }

    if (!ec)
        pipe.async_read_some(buffer.data(0, buffer_size), [this](const error_code &ec, std::size_t n) {
            read_loop(ec, n);
        });
}