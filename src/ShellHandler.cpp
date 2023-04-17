#include "ShellHandler.hpp"

#include <sstream>
#include <iostream>


ShellHandler::ShellHandler(OS operating_system) {
    std::string shell_command{};
    if(operating_system == OS::WINDOWS){
        shell_command = "cmd.exe";
    } else if(operating_system == OS::POSIX) {
        shell_command = "bash -i";
    }
    else if (operating_system == OS::MAC){
        shell_command = "zsh";
    } else{
        throw std::runtime_error("UNKNOWN OPERATING SYSTEM");
    }
    shell = bp::child(shell_command, bp::std_in < in, bp::std_out > out, bp::std_err > err);
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ShellHandler::write(const std::string &input) {
    in<<input<<std::endl;
}

std::string ShellHandler::readSTDOUT() {
    std::string line{};
    std::stringstream ss{};
    while (std::getline(out, line) && !line.empty()) {
        std::cout<<line<<std::endl;
        ss << line << "\n";
    }
    return ss.str();
}

std::string ShellHandler::readSTDERR() {
    std::string line{};
    std::stringstream ss{};

    while (std::getline(err, line) && !line.empty()) {
        std::cout<<line<<std::endl;
        ss << line << std::endl;
    }
    return ss.str();
}

ShellHandler::~ShellHandler() {
    if(shell.running()){
        shell.terminate();
    }
}
