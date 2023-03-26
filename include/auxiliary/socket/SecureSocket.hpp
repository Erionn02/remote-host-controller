#pragma once

#include "ZMQSocket.hpp"

class SecureSocket : public ISocket{
public:
    SecureSocket(std::unique_ptr<ISocket> socket);

    void bind(const std::string &address) override;

    void connect(const std::string &address) override;

    void send(zmq::message_t &message, zmq::send_flags flags) override;

    void send(zmq::multipart_t &message) override;

    zmq::recv_result_t recv(zmq::message_t &message) override;

    void recv(zmq::multipart_t &message) override;

    void setsockopt(int option, int option_value) override;

    int getsockopt(int option) override;

private:


    std::unique_ptr<ISocket> socket;
};


