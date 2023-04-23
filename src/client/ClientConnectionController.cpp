#include "client/ClientConnectionController.hpp"
#include "Utils.hpp"
#include "auxiliary/portable/PortableGetChar.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <iostream>


ClientConnectionController::ClientConnectionController(std::unique_ptr<SecureSocket> command_socket,
                                                       const std::string &peer_address,
                                                       std::unique_ptr<SecureSocket> response_socket,
                                                       const std::string &bind_address)
        : command_socket(std::move(command_socket)), data_socket(std::move(response_socket)) {
    this->command_socket->connect(peer_address);
    std::chrono::milliseconds timeout{4000};
    this->data_socket->setsockopt(ZMQ_RCVTIMEO, static_cast<int>(timeout.count()));
    this->data_socket->bind(bind_address);
    spdlog::debug("Client connected to {}", peer_address);
    spdlog::debug("Client bound to {}", bind_address);
}

void ClientConnectionController::stopHook() {
    if (receive_data_thread.joinable()) {
        receive_data_thread.join();
    }
}

void ClientConnectionController::startUpHook() {
    command_socket->sendEncryptedAESKey();
    receive_data_thread = std::jthread{[this] {
        while (is_running) {
            receiveWorkerLoop();
        }
    }};
}

void ClientConnectionController::receiveWorkerLoop() {
    zmq::multipart_t terminals_lines{};
    if (data_socket->recv(terminals_lines)) {
        for (auto &line: terminals_lines) {
            auto json = nlohmann::json::parse(std::string_view(line.data<char>(), line.size()));
            stream(json.at(JsonStructure::STREAM_TYPE_KEY)) << json.at(JsonStructure::CONTENT_KEY).get<std::string>();
        }
    }
}

void ClientConnectionController::workerLoop() {
    char c = portable::p_getChar();
    zmq::message_t message{&c, sizeof(c)};
    command_socket->send(message);

    zmq::message_t ack{};
    command_socket->recv(ack);
}

std::ostream &ClientConnectionController::stream(const std::string &stream_type) {
    return stream_type == JsonStructure::STDOUT ? std::cout : std::cerr;
}

