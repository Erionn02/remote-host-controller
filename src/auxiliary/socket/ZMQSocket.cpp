#include "auxiliary/socket/ZMQSocket.hpp"

ZMQSocket::ZMQSocket(zmq::socket_type socket_type) : socket(context, socket_type){
}


void ZMQSocket::bind(const std::string &address) {
    last_used_address = address;
    socket.bind(address);
}

void ZMQSocket::connect(const std::string &address) {
    last_used_address = address;
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
    socket.getsockopt(option,value,value_size);
}

void ZMQSocket::unbind() {
    socket.unbind(last_used_address);
}

void ZMQSocket::disconnect() {
    socket.disconnect(last_used_address);
}
