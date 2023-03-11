#pragma once

#include <zmq.hpp>
#include <zmq_addon.hpp>

#include <string>

class ISocket{
public:
    virtual ~ISocket()=default;

    virtual void bind(const std::string& address) = 0;
    virtual void connect(const std::string& address) = 0;
    virtual void send(zmq::message_t message, zmq::send_flags flags = zmq::send_flags::none) = 0;
    virtual void send(zmq::multipart_t message) = 0;
    virtual zmq::recv_result_t recv(zmq::message_t &message) = 0;
    virtual void recv(zmq::multipart_t &message) = 0;
    virtual void setsockopt(int option, int option_value) = 0;
};