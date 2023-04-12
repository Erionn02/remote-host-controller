#include <gtest/gtest.h>

#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

#include <thread>

using namespace ::testing;

struct SecureSocketTest : public Test {
    SecureSocket socket_client{std::make_unique<ZMQSocket>(zmq::socket_type::req)};
    SecureSocket socket_server{std::make_unique<ZMQSocket>(zmq::socket_type::rep)};
    std::string some_data{"This is some random data"};
    std::string address{"tcp://127.0.0.1:2137"};

    void exchangeKeys() {
        zmq::message_t message{"dummy data"};
        std::jthread t{[&] {
            socket_client.send(message);
        }};

        zmq::message_t received_message{};
        socket_server.recv(received_message);
        socket_server.send(received_message);
        socket_client.recv(received_message);
    }

    void TearDown() override {
        try{
            socket_server.unbind();
            socket_client.disconnect();
        } catch(...){}
    }
};


TEST_F(SecureSocketTest, communicationWithSingleMessageWorks) {
    //given
    zmq::message_t message{some_data};

    socket_server.bind(address);
    socket_client.connect(address);

    //when
    std::jthread t{[&] {
        socket_client.send(message);
    }};

    zmq::message_t received_message{};
    socket_server.recv(received_message);

    //then
    ASSERT_EQ(received_message.to_string(), some_data);
}


TEST_F(SecureSocketTest, communicationWithMultiMessageWorks) {
    //given
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
    std::jthread t{[&] {
        socket_client.send(multi_message);
    }};

    zmq::multipart_t received_messages{};
    socket_server.recv(received_messages);

    //then
    ASSERT_EQ(received_messages.size(), 2);
    ASSERT_EQ(received_messages.at(0).to_string(), some_data);
    ASSERT_EQ(received_messages.at(1).to_string(), some_different_data);
}

TEST_F(SecureSocketTest, canExchangeMessagesMultipleTimes) {
    //given
    socket_server.bind(address);
    socket_client.connect(address);
    exchangeKeys();


    // when
    for (int i = 0; i < 10; ++i) {
        zmq::message_t m{some_data};
        socket_client.send(m);
        zmq::message_t received{};
        socket_server.recv(received);
        zmq::message_t ack{"ack"};
        socket_server.send(ack);
        socket_client.recv(ack);

        //then
        ASSERT_EQ(received.to_string(), some_data);
    }
}

TEST_F(SecureSocketTest, doesNotExchangeDataWithEarsDropper) {
    //given
    socket_server.bind(address);
    socket_client.connect(address);
    exchangeKeys();

    ZMQSocket ears_dropper{zmq::socket_type::req};
    ears_dropper.connect(address);
    zmq::message_t user_data{some_data};

    // when
    std::jthread j{[&] {
        ASSERT_TRUE(ears_dropper.send(user_data));
    }
    };

    // then
    zmq::message_t m{};
    ASSERT_FALSE(socket_server.recv(m));
}

TEST_F(SecureSocketTest, pushPullSocketCommunication) {
    // given
    AESCryptographer cryptographer{};
    auto& key = cryptographer.getKey();
    auto& vec = cryptographer.getInitVector();
    SecureSocket pull_socket{zmq::socket_type::pull, key, vec};
    SecureSocket push_socket{zmq::socket_type::push, key, vec};
    pull_socket.bind(address);
    push_socket.connect(address);

    // when
    zmq::message_t send_message{some_data}, recv_message{};
    push_socket.send(send_message);
    pull_socket.recv(recv_message);

    //then
    ASSERT_EQ(recv_message.to_string(), some_data);
}


TEST_F(SecureSocketTest, lol) {
    // given


    // when
    ZMQSocket socket{zmq::socket_type::pull};
    socket.bind("tcp://0.0.0.0:*");

    char address[50];
    std::size_t size = sizeof(address);
    socket.getsockopt(ZMQ_LAST_ENDPOINT, &address, &size);
    std::cout<<address;
}
TEST_F(SecureSocketTest, getsockoptKeyAndInitVectorTest) {
    // given
    AESCryptographer cryptographer{};
    auto& key = cryptographer.getKey();
    auto& vec = cryptographer.getInitVector();
    auto new_key_buffer = std::make_unique<char[]>(key.size());
    auto new_vec_buffer = std::make_unique<char[]>(vec.size());

    // when
    SecureSocket socket{zmq::socket_type::push, key, vec};
    auto key_size = key.size();
    auto vec_size = vec.size();
    socket.getsockopt(AES_KEY, reinterpret_cast<void*>(new_key_buffer.get()), &key_size);
    socket.getsockopt(AES_VEC, reinterpret_cast<void*>(new_vec_buffer.get()), &vec_size);

    //then
    ASSERT_EQ(std::memcmp(new_key_buffer.get(), key.data(), key_size), 0);
    ASSERT_EQ(std::memcmp(new_vec_buffer.get(), vec.data(), vec_size), 0);
}