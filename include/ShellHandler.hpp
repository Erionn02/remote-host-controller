#pragma once

#include <string>

#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <zmq_addon.hpp>

namespace bp = boost::process;

enum class OS {
    POSIX,
    WINDOWS,
    MAC
};

class ShellHandler {
public:
    ShellHandler() = default;
    ShellHandler(ShellHandler&&) = default;
    ShellHandler& operator=(ShellHandler&&) = default;
    ~ShellHandler();
    explicit ShellHandler(OS operating_system);

    void sendSignal(int signal);
    void write(const std::string& input);
    zmq::multipart_t read();

private:
    using RAIIFile = std::unique_ptr<FILE, decltype(&fclose)>;
    using CharBuffer = std::unique_ptr<char, decltype(&free)>;
    bp::opstream in{};
    bp::child shell;
    std::string output_filename{"output.txt"};
    RAIIFile out_stream{NULL, fclose};
    ssize_t last_read_mark{0};
};


