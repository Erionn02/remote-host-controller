#include "RemoteHostConnectionController.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <iostream>

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


RemoteHostConnectionController::RemoteHostConnectionController(std::unique_ptr<ISocket> command_socket,
                                                               const std::string &command_socket_address,
                                                               std::unique_ptr<ISocket> response_socket,
                                                               const std::string &peers_address)
        : command_socket(std::move(command_socket)), response_socket(std::move(response_socket)) {
    std::chrono::milliseconds timeout{1000};
    this->command_socket->setsockopt(ZMQ_RCVTIMEO, static_cast<int>(timeout.count()));
    this->command_socket->bind(fmt::format("tcp://{}:*", command_socket_address));
    this->response_socket->connect(peers_address);
}

std::string RemoteHostConnectionController::getBoundAddress() {
    return command_socket->getLastEndpoint();
}

void RemoteHostConnectionController::workerLoop() {
    zmq::message_t message{};
    if(command_socket->recv(message)){
        zmq::message_t response{"Some response"};
        response_socket->send(response);
    }
}
