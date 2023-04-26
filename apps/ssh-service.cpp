#include "remote/RemoteHostListenerService.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"
#include "PeerFactory.hpp"
#include "client/ServiceConnector.hpp"

#include <spdlog/spdlog.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <chrono>

std::size_t MY_SSH_DEFAULT_PORT{2222};

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    if (argc == 1) {
        auto secure_socket = std::make_unique<SecureSocket>(zmq::socket_type::rep);
        RemoteHostListenerService service{std::move(secure_socket), "192.168.100.12", MY_SSH_DEFAULT_PORT};
        spdlog::info("STARTING LISTENER SERVICE... ");
        service.start();
        spdlog::info("LISTENER SERVICE STARTED SUCCESSFULLY");
        std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());
    } else if (argc == 2) {
        std::string service_address = argv[1];
        std::string my_address{ZMQSocket::getAvailableBindAddress("192.168.100.12")};
        spdlog::info("CONNECTING TO SERVICE {}", service_address);
        auto remote_address = ServiceConnector::getRemoteHostAddress(my_address, service_address, MY_SSH_DEFAULT_PORT);
        spdlog::info("CONNECTED TO SERVICE, GOT NEW REMOTE ADDRESS {}", remote_address);
        auto client_controller = PeerFactory::createClientConnectionController(remote_address, my_address);
        client_controller->start();
        std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>::max());
    }
   return 0;
}
