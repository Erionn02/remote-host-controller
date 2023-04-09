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


RemoteHostConnectionController::RemoteHostConnectionController(std::unique_ptr<ISocket> socket, const std::string &address_without_port)
        : socket(std::move(socket)) {
    this->socket->setsockopt(ZMQ_RCVTIMEO, 1000);
    this->socket->bind(fmt::format("tcp://{}:*", address_without_port));
}

std::string RemoteHostConnectionController::getBoundAddress() {
    char address[50];
    std::size_t size = sizeof(address);
    socket->getsockopt(ZMQ_LAST_ENDPOINT, &address, &size);
    return address;
}

void RemoteHostConnectionController::workerLoop() {
    zmq::message_t message{};
    if(socket->recv(message)){
        auto output = exec(message.to_string().data());
        spdlog::info("COMMAND: {}, output: \n{}",message.to_string(), output);
        if(output.empty()) output = "elo";
        zmq::message_t response{output};
        socket->send(response);
    }
}
