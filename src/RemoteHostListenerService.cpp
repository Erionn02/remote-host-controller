#include "RemoteHostListenerService.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>


RemoteHostListenerService::RemoteHostListenerService(std::unique_ptr<ISocket> socket, const std::string &bind_address, std::size_t port)
        : socket(std::move(socket)), address(bind_address) {
    this->socket->setsockopt(ZMQ_RCVTIMEO, 1000);
    this->socket->bind(fmt::format("tcp://{}:{}", address, port));
}

void RemoteHostListenerService::workerLoop() {
    if (isNewConnectionRequestReceived()) {
        auto normal_socket = std::make_unique<ZMQSocket>(zmq::socket_type::rep);
        auto secure_socket = std::make_unique<SecureSocket>(std::move(normal_socket));
        //TODO replace later with factory call
        auto connection = std::make_unique<RemoteHostConnectionController>(std::move(secure_socket), address);
        auto bound_address = connection->getBoundAddress();

        spdlog::info("Created new connection, bind address: {}", bound_address);
        connection->start();
        connections.emplace_back(std::move(connection));

        zmq::message_t address_to_connect_to{bound_address};
        socket->send(address_to_connect_to);
    }
    removeStoppedConnections();
}

bool RemoteHostListenerService::isNewConnectionRequestReceived() {
    zmq::message_t dummy_message{};
    return socket->recv(dummy_message);
}

void RemoteHostListenerService::removeStoppedConnections() {
    std::erase_if(connections, [](auto& connection){return !connection->isRunning();});
}
