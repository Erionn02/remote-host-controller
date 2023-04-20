#pragma once
#include "StreamReader.hpp"
#include "Types.hpp"

#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <zmq_addon.hpp>

#include <string>

namespace bp = boost::process;


class ShellHandler {
public:
    ShellHandler() = default;
    ShellHandler(ShellHandler&&) = default;
//    ShellHandler& operator=(ShellHandler&&) = default;
    ~ShellHandler();
    explicit ShellHandler(OS operating_system);

    void sendSignal(int signal);
    void write(const std::string& input);
    std::vector<std::string> read();

private:
    bp::opstream in{};
    bp::child shell;
    std::unique_ptr<asio::io_context> ios{std::make_unique<asio::io_context>()}; //yeah, I know.. but the move constructor of io_context is banned so..
    StreamOutputQueue stream_outputs{};
    StreamReader p_stdout{*ios, stream_outputs, JsonStructure::STDOUT};
    StreamReader p_stderr{*ios, stream_outputs, JsonStructure::STDERR};
    std::jthread read_thread;
};


