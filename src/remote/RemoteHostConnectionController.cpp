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
        std::size_t size{16};
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
    shell = ShellHandler{OS::POSIX};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    response_thread = std::jthread{[this] {
        while (is_running) {
            commandOutputWorkerLoop();
        }
    }};
}

void RemoteHostConnectionController::commandOutputWorkerLoop() {
    //do some processing and then
    auto stdout_content = shell.readSTDOUT();
//    auto stderr_content = shell.readSTDERR();
    zmq::message_t response{stdout_content};
    response_socket->send(response);

//    if (!stderr_content.empty() || !stderr_content.empty()) {
//
//    }

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
