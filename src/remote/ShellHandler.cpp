#include "remote/ShellHandler.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

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
            ios->run();
        }
    }};
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ShellHandler::sendSignal(int) {

}

void ShellHandler::write(const std::string &input) {
    in<<input<<std::endl;
}

std::vector<std::string> ShellHandler::read() {
    std::vector<std::string> output_vec{};
    StreamOutput output;
    while(stream_outputs.try_pop(output)) {
        nlohmann::json json_output{};
        json_output.emplace(JsonStructure::STREAM_TYPE_KEY, output.stream_type);
        json_output.emplace(JsonStructure::CONTENT_KEY, std::move(output.content));
        output_vec.emplace_back(json_output.dump());
    }
    return output_vec;
}

ShellHandler::~ShellHandler() {
    if(shell.running()){
        shell.terminate();
    }
}


