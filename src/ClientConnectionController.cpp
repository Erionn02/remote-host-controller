#include "ClientConnectionController.hpp"

#include <iostream>



ClientConnectionController::ClientConnectionController(std::unique_ptr<ISocket> command_socket, const std::string &peer_address,
                                                       std::unique_ptr<ISocket> response_socket,const std::string &bind_address)
        : command_socket(std::move(command_socket)), data_socket(std::move(response_socket)) {
    this->command_socket->connect(peer_address);
    std::chrono::milliseconds timeout{1000};
    this->data_socket->setsockopt(ZMQ_RCVTIMEO, static_cast<int>(timeout.count()));
    this->data_socket->bind(bind_address);
}

void ClientConnectionController::workerLoop() {
    std::string command{};
    std::cout<<"ENTER COMMAND: \n";
    std::getline(std::cin, command);
    std::cout<<"COMMAND ENTERED.\n";
    zmq::message_t message{command};
    command_socket->send(message);

    zmq::message_t response{};
    command_socket->recv(response);
    std::cout << response.to_string();
}
