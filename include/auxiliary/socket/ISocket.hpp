#pragma once

#include <zmq.hpp>
#include <zmq_addon.hpp>

#include <string>

class ISocket {
public:
    virtual ~ISocket() = default;

    virtual void bind(const std::string &address) = 0;

    virtual void unbind() = 0;

    virtual void connect(const std::string &address) = 0;

    virtual void disconnect() = 0;

    virtual bool send(zmq::message_t &message, zmq::send_flags flags = zmq::send_flags::none) = 0;

    virtual bool send(zmq::multipart_t &message) = 0;

    virtual bool recv(zmq::message_t &message) = 0;

    virtual bool recv(zmq::multipart_t &message) = 0;

    virtual void setsockopt(int option, int option_value) = 0;

    virtual int getsockopt(int option) = 0;

    virtual void getsockopt(int option, void *value, size_t *value_size) = 0;
};