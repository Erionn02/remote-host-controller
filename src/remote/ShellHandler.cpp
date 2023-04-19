#include "remote/ShellHandler.hpp"

#include <spdlog/spdlog.h>

#include <sstream>


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
    spdlog::debug("bp::child");
    shell = bp::child(shell_command, bp::std_in < in, bp::std_out > p_stdout.getPipe(), bp::std_err > p_stderr.getPipe());
    read_thread = std::jthread{[this]{
        while(shell.running()){
            ios->run_for(std::chrono::milliseconds(200));
            std::this_thread::yield();
        }
    }};
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ShellHandler::sendSignal(int) {

}

void ShellHandler::write(const std::string &input) {
    in<<input<<std::endl;
}

std::vector<StreamOutput> ShellHandler::read() {
    std::vector<StreamOutput> output_vec{};
    StreamOutput output;
    while(stream_outputs.try_pop(output)){
        output_vec.emplace_back(std::move(output));
    }
    return output_vec;
}

ShellHandler::~ShellHandler() {
    if(shell.running()){
        shell.terminate();
    }
}


