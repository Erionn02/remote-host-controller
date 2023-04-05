#pragma once
#include <gmock/gmock.h>

#include "auxiliary/socket/ISocket.hpp"

class SocketMock : public ISocket{
    MOCK_METHOD(void, bind, (const std::string& address), (override));
    MOCK_METHOD(void, connect, (const std::string& address), (override));
    MOCK_METHOD(void, send, (zmq::message_t message, zmq::send_flags flags), (override));
    MOCK_METHOD(void, send, (zmq::multipart_t message), (override));
    MOCK_METHOD(zmq::recv_result_t, recv, (zmq::message_t& message), (override));
    MOCK_METHOD(void, recv, (zmq::multipart_t& message), (override));
    MOCK_METHOD(void, setsockopt, (int option, int option_value), (override));
    MOCK_METHOD(int, getsockopt, (int option), (override));
};
