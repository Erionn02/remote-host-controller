#include "auxiliary/socket/ZMQSocket.hpp"

ZMQSocket::ZMQSocket(zmq::socket_type socket_type) : socket(context, socket_type){
}


void ZMQSocket::bind(const std::string &address) {
    socket.bind(address);
}

void ZMQSocket::connect(const std::string &address) {
    socket.connect(address);
}

void ZMQSocket::send(zmq::message_t message, zmq::send_flags flags) {
    socket.send(message, flags);
}

void ZMQSocket::send(zmq::multipart_t message) {
    message.send(socket);
}

zmq::recv_result_t ZMQSocket::recv(zmq::message_t &message) {
    return socket.recv(message);
}

void ZMQSocket::recv(zmq::multipart_t &message) {
    message.recv(socket);
}

void ZMQSocket::setsockopt(int option, int option_value) {
    socket.setsockopt(option, option_value);
}

int ZMQSocket::getsockopt(int option) {
    return socket.getsockopt<int>(option);
}
