#include <gtest/gtest.h>

#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

#include <thread>


TEST(SecureSocketTest, tmp) {
    //given
    SecureSocket socket_client{std::make_unique<ZMQSocket>(zmq::socket_type::req)};
    auto serialized = socket_client.encryptAESKey();
    auto [key, vector] = socket_client.decryptAES(serialized);

    ASSERT_EQ(key, socket_client.aes.getKey());
    ASSERT_EQ(vector, socket_client.aes.getInitVector());

    auto serialized_pub = socket_client.serializePublicKey();
    auto deserialized_key = socket_client.deserializePublicKey(serialized_pub);

    ASSERT_EQ(deserialized_key.GetPublicExponent(),socket_client.rsa.getPublicKey().GetPublicExponent());
    ASSERT_EQ(deserialized_key.GetModulus(),socket_client.rsa.getPublicKey().GetModulus());
}



TEST(SecureSocketTest, everythingWorksFine) {
    //given
    SecureSocket socket_client{std::make_unique<ZMQSocket>(zmq::socket_type::req)};
    SecureSocket socket_server{std::make_unique<ZMQSocket>(zmq::socket_type::rep)};
    std::string some_data{"LOLOLOLOL"};
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
