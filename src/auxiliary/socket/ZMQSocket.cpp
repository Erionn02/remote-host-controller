#include "auxiliary/socket/ZMQSocket.hpp"

#include <fmt/format.h>
#include <boost/algorithm/string.hpp>

#include <ranges>


ZMQSocket::ZMQSocket(zmq::socket_type socket_type) : socket(context, socket_type) {
}


void ZMQSocket::bind(const std::string &address) {
    socket.bind(address);
}

void ZMQSocket::connect(const std::string &address) {
    last_connected_address = address;
    socket.connect(address);
}

bool ZMQSocket::send(zmq::message_t &message, zmq::send_flags flags) {
    return socket.send(message, flags).has_value();
}

bool ZMQSocket::send(zmq::multipart_t &message) {
    return message.send(socket);
}

bool ZMQSocket::recv(zmq::message_t &message) {
    return socket.recv(message).has_value();
}

bool ZMQSocket::recv(zmq::multipart_t &message) {
    message.recv(socket);
    return !message.empty();
}

void ZMQSocket::setsockopt(int option, int option_value) {
    socket.setsockopt(option, option_value);
}

int ZMQSocket::getsockopt(int option) {
    return socket.getsockopt<int>(option);
}

void ZMQSocket::getsockopt(int option, void *value, size_t *value_size) {
    socket.getsockopt(option, value, value_size);
}

void ZMQSocket::unbind() {
    socket.unbind(getLastEndpoint());
}

void ZMQSocket::disconnect() {
    socket.disconnect(last_connected_address);
}

std::string ZMQSocket::getLastEndpoint() {
    char address[50];
    std::size_t size = sizeof(address);
    socket.getsockopt(ZMQ_LAST_ENDPOINT, &address, &size);
    return address;
}

std::string ZMQSocket::getAvailableBindAddress(const std::string &ip_address) {
    ZMQSocket sock{zmq::socket_type::rep};
    sock.bind(fmt::format("tcp://{}:*", ip_address));
    auto last_endpoint = sock.getLastEndpoint();
    sock.unbind();
    return last_endpoint;
}

std::vector<std::string> split(std::string tokenized_string, const std::string &delimiter) {
    std::vector<std::string> tokens{};
    auto delim_length = delimiter.length();
    std::size_t position = tokenized_string.find(delimiter);
    while (position != std::string::npos) {
        auto token = tokenized_string.substr(0, position);
        tokens.push_back(token);
        tokenized_string.erase(0, position + delim_length);
        position = tokenized_string.find(delimiter);
    }
    tokens.push_back(tokenized_string);
    return tokens;
};


bool ZMQSocket::isAddressWithPortAndProtocolValid(const std::string &address_with_port_and_protocol) {
    auto tokens = split(address_with_port_and_protocol, "://");
    if (tokens.size() != 2) {
        return false;
    }

    if (tokens[0] != "tcp") {
        return false;
    }

    auto address_and_port = split(tokens[1], ":");
    if (address_and_port.size() != 2) {
        return false;
    }

    std::string &address = address_and_port[0];
    std::string &port = address_and_port[1];

    auto is_number = [](const std::string &maybe_num) {
        return std::ranges::all_of(maybe_num, [](char c) { return std::isdigit(c); });
    };

    int max_port_number = 65535;
    if (!is_number(port) || std::stoi(port) <= 0 || std::stoi(port) > max_port_number) {
        return false;
    }

    auto address_octets = split(address, ".");
    if (address_octets.size() != 4) {
        return false;
    }

    return std::ranges::all_of(address_octets, [&is_number](auto &octet) {
        return is_number(octet) && std::stoi(octet) >= 0 && std::stoi(octet) <= 255;
    });
}
