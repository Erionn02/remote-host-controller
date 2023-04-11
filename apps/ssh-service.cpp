#include "RemoteHostListenerService.hpp"
#include "ClientConnectionController.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

#include <spdlog/spdlog.h>


#include <string>
#include <memory>
#include <zmq.hpp>
#include <chrono>
#include <unordered_map>
#include <iostream>

#include <boost/process.hpp>
#include <boost/process/pipe.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

namespace bp = boost::process;
namespace bio = boost::iostreams;

std::size_t MY_SSH_DEFAULT_PORT{2222};

int main(int argc, char *argv[]) {
//int main(){
    spdlog::set_level(spdlog::level::debug);
    if (argc == 1) {
        auto underlying_socket = std::make_unique<ZMQSocket>(zmq::socket_type::rep);
        auto secure_socket = std::make_unique<SecureSocket>(std::move(underlying_socket));
        RemoteHostListenerService service{std::move(secure_socket), "127.0.0.1", MY_SSH_DEFAULT_PORT};
        spdlog::info("STARTED LISTENER SERVICE");
        service.start();
        std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());
    } else if (argc == 2) {
        std::string address = argv[1];
        auto underlying_socket = std::make_unique<ZMQSocket>(zmq::socket_type::req);
        auto underlying_socket_2 = std::make_unique<ZMQSocket>(zmq::socket_type::req);
        auto secure_socket = std::make_unique<SecureSocket>(std::move(underlying_socket));
        auto secure_socket_2 = std::make_unique<SecureSocket>(std::move(underlying_socket_2));
        secure_socket_2->connect(fmt::format("tcp://{}:{}", argv[1], MY_SSH_DEFAULT_PORT));
        zmq::message_t dummy1{"lol"}, remote_address{};
        secure_socket_2->send(dummy1);
        secure_socket_2->recv(remote_address);
        ClientConnectionController controller{std::move(secure_socket), remote_address.to_string()};

        spdlog::info("STARTED CLIENT CONTROLLER");
        controller.start();
        std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());
    }

//    bp::ipstream out;
//    bp::ipstream err;
//    bp::opstream in;
//
//
//    bp::child process("bash -i", bp::std_in < in, bp::std_out > out, bp::std_err > err);
//
//    // Wait a second for the process to start up
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        // Loop for input and output
//        while (process.running()) {
//            std::string input {};
//            std::getline(std::cin, input);
//            in << input<<std::endl;
//            std::this_thread::sleep_for(std::chrono::milliseconds (40));
//
//            std::string line{};
//            std::stringstream ss;
//
//            while (!out.eof() && out.good() && std::getline(out, line) && !line.empty()) {
//                ss << line << std::endl;
//            }
//            std::cout<<ss.str();
//            // Read from both output and error streams
//            std::string output, error;
//            while (std::getline(out, output) || std::getline(err, error)) {
//                if (!output.empty()) {
//                    std::cout << "Output: " << output << std::endl;
//                }
//                if (!error.empty()) {
//                    std::cout << "Error: " << error << std::endl;
//                }
//                // If both streams are empty, break out of the loop
//                if (output.empty() && error.empty()) {
//                    break;
//                }
//            }
//        }

        // Wait for the process to finish
//        process.wait();

    return 0;
}
