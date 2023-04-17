#pragma once

#include <string>

#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>

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

    void write(const std::string& input);
    std::string readSTDOUT();
    std::string readSTDERR();

private:
    bp::ipstream out{};
    bp::ipstream err{};
    bp::opstream in{};
    bp::child shell;
};


