#include "auxiliary/socket/SecureSocket.hpp"


SecureSocket::SecureSocket(std::unique_ptr<ISocket> socket) : socket(std::move(socket)){
    int socket_type = this->socket->getsockopt(ZMQ_TYPE);
    if(socket_type!=ZMQ_REP && socket_type!=ZMQ_REQ){
        throw std::runtime_error("For SecureSocket the only available types of underlying socket"
                                 " are zmq::socket_type::rep and zmq::socket_type::req.");
    }
}

void SecureSocket::bind(const std::string &address) {
    socket->bind(address);
}

void SecureSocket::connect(const std::string &address) {
    socket->connect(address);
}

void SecureSocket::send(zmq::message_t message, zmq::send_flags flags) {
    socket->send(std::move(message),flags);
}

void SecureSocket::send(zmq::multipart_t message) {
    socket->send(std::move(message));
}

zmq::recv_result_t SecureSocket::recv(zmq::message_t &message) {
    return socket->recv(message);
}

void SecureSocket::recv(zmq::multipart_t &message) {
    return socket->recv(message);
}

void SecureSocket::setsockopt(int option, int option_value) {
    socket->setsockopt(option, option_value);
}

int SecureSocket::getsockopt(int option) {
    return socket->getsockopt(option);
}

