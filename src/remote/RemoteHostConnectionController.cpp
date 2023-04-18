#include "remote/RemoteHostConnectionController.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <iostream>


RemoteHostConnectionController::RemoteHostConnectionController(std::unique_ptr<SecureSocket> command_socket,
                                                               const std::string &command_socket_address,
                                                               std::unique_ptr<SecureSocket> response_socket,
                                                               const std::string &peers_address)
        : command_socket(std::move(command_socket)), response_socket(std::move(response_socket)) {
    std::chrono::milliseconds timeout{4000};
    this->command_socket->setsockopt(ZMQ_RCVTIMEO, static_cast<int>(timeout.count()));
    this->command_socket->bind(fmt::format("tcp://{}:*", command_socket_address));
    this->response_socket->connect(peers_address);
    spdlog::debug("Remote connected to {}", peers_address);
    spdlog::debug("Remote bound to {}", this->command_socket->getLastEndpoint());
}

void RemoteHostConnectionController::startUpHook() {
    if (command_socket->receiveAESKey()) {
        std::size_t size{CryptoPP::AES::DEFAULT_KEYLENGTH};
        auto key_buffer = std::make_unique<unsigned char[]>(size);
        auto init_vec_buffer = std::make_unique<unsigned char[]>(size);
        command_socket->getsockopt(AES_KEY, key_buffer.get(), &size);
        command_socket->getsockopt(AES_VEC, init_vec_buffer.get(), &size);

        response_socket->setsockopt(AES_KEY, key_buffer.get(), size);
        response_socket->setsockopt(AES_VEC, init_vec_buffer.get(), size);
        auto last_endpoint = response_socket->getLastEndpoint();
        response_socket->disconnect();
        response_socket->connect(last_endpoint);
    } else {
        throw std::runtime_error("Receiving AES key failed.");
    }
    spdlog::debug("Creating shell instance.");
    shell = ShellHandler{OS::POSIX};
    spdlog::debug("Shell instance was created successfully!");
    response_thread = std::jthread{[this] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        while (is_running) {
            commandOutputWorkerLoop();
        }
    }};
}

void RemoteHostConnectionController::commandOutputWorkerLoop() {
    auto terminals_content = shell.read();
    if(!terminals_content.empty()){
        for(auto& line: terminals_content) {
            std::cout<<line.to_string();
        }
        response_socket->send(terminals_content);
    } else {
        std::this_thread::yield();
    }
}

void RemoteHostConnectionController::stopHook() {
    if (response_thread.joinable()) {
        response_thread.join();
    }
}


std::string RemoteHostConnectionController::getBoundAddress() {
    return command_socket->getLastEndpoint();
}

void RemoteHostConnectionController::workerLoop() {
    zmq::message_t message{};
    if (command_socket->recv(message)) {
        zmq::message_t ack{"ACK"};
        command_socket->send(ack);
        shell.write(message.to_string());
    }
}
