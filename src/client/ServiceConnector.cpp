#include "client/ServiceConnector.hpp"

#include <fmt/format.h>

std::string
ServiceConnector::getRemoteHostAddress(const std::string &my_address, const std::string &remote_host_service_address,
                                       std::size_t port) {
    SecureSocket ssh_service_connector{zmq::socket_type::req};
    ssh_service_connector.connect(fmt::format("tcp://{}:{}", remote_host_service_address, port));

    zmq::message_t client_socket_address_message{my_address};
    ssh_service_connector.send(client_socket_address_message);

    zmq::message_t remote_address{};
    ssh_service_connector.recv(remote_address);
    ssh_service_connector.disconnect();

    return remote_address.to_string();
}
