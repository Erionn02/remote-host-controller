#include "PeerToPeerConnection.hpp"

#include <fmt/format.h>

PeerToPeerConnection::PeerToPeerConnection(std::unique_ptr<ISocket> socket, const std::string &address_without_port)
        : socket(std::move(socket)) {
    this->socket->bind(fmt::format("tcp://{}:*", address_without_port));
}

std::size_t PeerToPeerConnection::getBoundPort() {
    char port[50];
    std::size_t size = sizeof(port);
    socket->getsockopt(ZMQ_LAST_ENDPOINT, &port, &size);
    return std::stoull(port);
}

void PeerToPeerConnection::workerLoop() {

}
