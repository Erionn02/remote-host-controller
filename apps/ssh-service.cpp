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

std::size_t MY_SSH_DEFAULT_PORT{2222};

int main(int argc, char *argv[]) {
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

    return 0;
}
