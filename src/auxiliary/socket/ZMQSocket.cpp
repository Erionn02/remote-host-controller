#include "auxiliary/socket/ZMQSocket.hpp"

ZMQSocket::ZMQSocket(zmq::socket_type socket_type) : socket(context, socket_type){
}


void ZMQSocket::bind(const std::string &address) {

}

void ZMQSocket::connect(const std::string &address) {

}

void ZMQSocket::send(zmq::message_t message, zmq::send_flags flags) {

}

void ZMQSocket::send(zmq::multipart_t message) {

}

void ZMQSocket::recv(zmq::message_t message, zmq::recv_flags flags) {

}

void ZMQSocket::recv(zmq::multipart_t message) {

}

void ZMQSocket::setsockopt(int option) {

}
