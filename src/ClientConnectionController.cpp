#include "ClientConnectionController.hpp"

#include <iostream>



ClientConnectionController::ClientConnectionController(std::unique_ptr<ISocket> socket,
                                                       const std::string &peer_address) : socket(std::move(socket)) {
    this->socket->connect(peer_address);
}

void ClientConnectionController::workerLoop() {
    std::string command{};
    std::cout<<"ENTER COMMAND: \n";
    std::getline(std::cin, command);
    std::cout<<"COMMAND ENTERED.\n";
    zmq::message_t message{command};
    socket->send(message);

    zmq::message_t response{};
    socket->recv(response);
    std::cout << response.to_string();
}
