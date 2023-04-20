#include "remote/StreamReader.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/process.hpp>

#include <iostream>
#include <thread>


int main() {
    asio::io_context ios;
    StreamOutputQueue outputs{};
    StreamReader pipeOut(ios, outputs, JsonStructure::STDOUT);
    StreamReader pipeErr(ios, outputs, JsonStructure::STDERR);

    bp::opstream in;

    bp::child c(
            "bash -i",
            bp::std_out > pipeOut.getPipe(),
            bp::std_err > pipeErr.getPipe(),
            bp::std_in < in,
            ios);


    std::jthread io_thread([&ios] { ios.run(); });
    std::jthread print_thread([&outputs, &c] {
        while (c.running()) {
            StreamOutput output;
            if (outputs.try_pop(output)) {
                if (output.stream_type == JsonStructure::STDOUT) {
                    std::cout << output.content;
                } else {
                    std::cerr << output.content;
                }
            }
            std::this_thread::yield();
        }
    });

    std::cout << "STARTING LOOP: \n";
    while (c.running()) {
        std::string input_command{"echo $RANDOM"};
        in << input_command << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }


    return 0;
}