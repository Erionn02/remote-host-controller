#include "ShellHandler.hpp"

#include <spdlog/spdlog.h>

#include <sstream>
#include <stdio.h>


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
    out_stream.reset(fopen(output_filename.c_str(), "w"));
    spdlog::debug("bp::child");
    shell = bp::child(shell_command, bp::std_in < in, bp::std_out > out_stream.get(), bp::std_err > out_stream.get());
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ShellHandler::sendSignal(int) {

}

void ShellHandler::write(const std::string &input) {
    in<<input<<std::endl;
}

zmq::multipart_t ShellHandler::read() {
    zmq::multipart_t lines{};
    std::size_t len{0};
    ssize_t bytes_read{0};
    CharBuffer line{NULL, free};
    char *line_ptr = line.get();
    // In order to see updates in stream, for some reason I cannot read from out_stream member and have to reopen file for read every time. Dirty, but works.
    RAIIFile output_stream_read{fopen(output_filename.c_str(), "r"), fclose};
    fseek(output_stream_read.get(), last_read_mark, SEEK_CUR);
    while ((bytes_read = getline(&line_ptr, &len, output_stream_read.get())) != EOF) {
        zmq::message_t line_m{line_ptr, static_cast<std::size_t>(bytes_read)};
        lines.add(std::move(line_m));
        last_read_mark += bytes_read;
    }
    return lines;
}

ShellHandler::~ShellHandler() {
    if(shell.running()){
        shell.terminate();
    }
}


