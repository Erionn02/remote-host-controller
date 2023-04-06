#include <gtest/gtest.h>

#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

#include <thread>


TEST(SecureSocketTest, communicationWithSingleMessageWorks) {
    //given
    SecureSocket socket_client{std::make_unique<ZMQSocket>(zmq::socket_type::req)};
    SecureSocket socket_server{std::make_unique<ZMQSocket>(zmq::socket_type::rep)};
    std::string some_data{"This is some random data"};
    std::string address{"tcp://127.0.0.1:2137"};
    zmq::message_t message{some_data};

    socket_server.bind(address);
    socket_client.connect(address);

    //when
    std::jthread t{[&]{
        socket_client.send(message);
        }};

    zmq::message_t received_message{};
    socket_server.recv(received_message);

    //then
    ASSERT_EQ(received_message.to_string(), some_data);
}


TEST(SecureSocketTest, communicationWithMultiMessageWorks) {
    //given
    SecureSocket socket_client{std::make_unique<ZMQSocket>(zmq::socket_type::req)};
    SecureSocket socket_server{std::make_unique<ZMQSocket>(zmq::socket_type::rep)};
    std::string some_data{"This is some random data"};
    std::string some_different_data{"This is some different random data"};
    std::string address{"tcp://127.0.0.1:2137"};

    zmq::multipart_t multi_message{};
    zmq::message_t m1{some_data};
    zmq::message_t m2{some_different_data};
    multi_message.add(std::move(m1));
    multi_message.add(std::move(m2));

    socket_server.bind(address);
    socket_client.connect(address);

    //when
    std::jthread t{[&]{
        socket_client.send(multi_message);
    }};

    zmq::multipart_t received_messages{};
    socket_server.recv(received_messages);

    //then
    ASSERT_EQ(received_messages.size(), 2);
    ASSERT_EQ(received_messages.at(0).to_string(), some_data);
    ASSERT_EQ(received_messages.at(1).to_string(), some_different_data);
}
