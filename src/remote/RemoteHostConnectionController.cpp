#include "remote/RemoteHostConnectionController.hpp"

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


RemoteHostConnectionController::RemoteHostConnectionController(std::unique_ptr<SecureSocket> command_socket,
                                                               const std::string &command_socket_address,
                                                               std::unique_ptr<SecureSocket> response_socket,
                                                               const std::string &peers_address)
        : command_socket(std::move(command_socket)), response_socket(std::move(response_socket)) {
    std::chrono::milliseconds timeout{4000};
    this->command_socket->setsockopt(ZMQ_RCVTIMEO, static_cast<int>(timeout.count()));
    this->command_socket->bind(fmt::format("tcp://{}:*", command_socket_address));
    this->response_socket->connect(peers_address);
}

void RemoteHostConnectionController::startUpHook() {
    if(command_socket->receiveAESKey()) {
        std::size_t size{16};
        auto key_buffer = std::make_unique<unsigned char[]>(size);
        auto init_vec_buffer = std::make_unique<unsigned char[]>(size);
        command_socket->getsockopt(AES_KEY, key_buffer.get(), &size);
        command_socket->getsockopt(AES_VEC, init_vec_buffer.get(), &size);
        response_socket->setsockopt(AES_KEY, key_buffer.get(), size);
        response_socket->setsockopt(AES_VEC, init_vec_buffer.get(), size);
    } else {
        throw std::runtime_error("Receiving AES key failed.");
    }
}


std::string RemoteHostConnectionController::getBoundAddress() {
    return command_socket->getLastEndpoint();
}

void RemoteHostConnectionController::workerLoop() {
    zmq::message_t message{};
    if(command_socket->recv(message)){
        zmq::message_t ack{"ACK"};
        command_socket->send(ack);

        zmq::message_t response{std::string("Some response")};
        response_socket->send(response);
    }
}

