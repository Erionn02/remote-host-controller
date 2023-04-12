#include "remote/RemoteHostListenerService.hpp"
#include "client/ClientConnectionController.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"
#include "PeerFactory.hpp"
#include "client/ServiceConnector.hpp"

#include <spdlog/spdlog.h>


#include <string>
#include <memory>
#include <zmq.hpp>
#include <chrono>
#include <unordered_map>
//#include <iostream>
//
//#include <boost/process.hpp>
//#include <boost/process/pipe.hpp>
//#include <boost/iostreams/stream.hpp>
//#include <boost/iostreams/tee.hpp>
//#include <boost/iostreams/device/file_descriptor.hpp>
//
//namespace bp = boost::process;
//namespace bio = boost::iostreams;

std::size_t MY_SSH_DEFAULT_PORT{2222};

int main(int argc, char *argv[]) {
//int main(){
    spdlog::set_level(spdlog::level::debug);
    if (argc == 1) {
        auto secure_socket = std::make_unique<SecureSocket>(zmq::socket_type::rep);
        RemoteHostListenerService service{std::move(secure_socket), "127.0.0.1", MY_SSH_DEFAULT_PORT};
        spdlog::info("STARTING LISTENER SERVICE... ");
        service.start();
        spdlog::info("LISTENER SERVICE STARTED SUCCESSFULLY");
        std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());
    } else if (argc == 2) {
        std::string service_address = argv[1];
        std::string my_address{"tcp://127.0.0.1:2137"};
        spdlog::info("CONNECTING TO SERVICE {}", service_address);
        auto remote_address = ServiceConnector::getRemoteHostAddress(my_address, service_address, MY_SSH_DEFAULT_PORT);
        spdlog::info("CONNECTED TO SERVICE, GOT NEW REMOTE ADDRESS {}", remote_address);

        auto controller = PeerFactory::createClientConnectionController(remote_address, my_address);

        spdlog::info("STARTING CLIENT CONTROLLER...");
        controller->start();
        spdlog::info("CLIENT CONTROLLER STARTED SUCCESSFULLY");
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
