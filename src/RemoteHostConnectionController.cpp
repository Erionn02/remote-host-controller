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


RemoteHostConnectionController::RemoteHostConnectionController(std::unique_ptr<ISocket> command_socket, const std::string &address_without_port)
        : command_socket(std::move(command_socket)) {
    this->command_socket->setsockopt(ZMQ_RCVTIMEO, 1000);
    this->command_socket->bind(fmt::format("tcp://{}:*", address_without_port));
}

std::string RemoteHostConnectionController::getBoundAddress() {
    char address[50];
    std::size_t size = sizeof(address);
    command_socket->getsockopt(ZMQ_LAST_ENDPOINT, &address, &size);
    return address;
}

void RemoteHostConnectionController::workerLoop() {
    zmq::message_t message{};
    if(command_socket->recv(message)){
        auto output = exec(message.to_string().data());
        spdlog::info("COMMAND: {}, output: \n{}",message.to_string(), output);
        if(output.empty()) output = "elo";
        zmq::message_t response{output};
        command_socket->send(response);
    }
}
