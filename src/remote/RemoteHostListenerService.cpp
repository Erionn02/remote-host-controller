#include "remote/RemoteHostListenerService.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"
#include "PeerFactory.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>


RemoteHostListenerService::RemoteHostListenerService(std::unique_ptr<ISocket> socket, const std::string &bind_address,
                                                     std::size_t port)
        : socket(std::move(socket)), address(bind_address) {
    this->socket->setsockopt(ZMQ_RCVTIMEO, 1000);
    this->socket->bind(fmt::format("tcp://{}:{}", address, port));
}

void RemoteHostListenerService::workerLoop() {
    zmq::message_t peers_address{};
    if (!isLimitOfConnectionsReached() && socket->recv(peers_address)) {
        zmq::message_t response{};
        if (ZMQSocket::isAddressWithPortAndProtocolValid(peers_address.to_string())) {
            auto connection = PeerFactory::createRemoteHostConnectionController(address, peers_address.to_string());
            auto bound_address = connection->getBoundAddress();
            spdlog::info("Created new connection, bind address: {}", bound_address);
            zmq::message_t address_to_connect_to{bound_address};
            socket->send(address_to_connect_to);

            connection->start();
            spdlog::info("Started connection {}", bound_address);
            connections.emplace_back(std::move(connection));

        } else {
            zmq::message_t error_response{"Got invalid address"};
            socket->send(error_response);
        }
        socket->setsockopt(EXCHANGED_KEYS, 0);
    }
    removeStoppedConnections();
}


void RemoteHostListenerService::removeStoppedConnections() {
    std::erase_if(connections, [](auto &connection) { return !connection->isRunning(); });
}

bool RemoteHostListenerService::isLimitOfConnectionsReached() {
    return connections.size() > limit_of_connections;
}
