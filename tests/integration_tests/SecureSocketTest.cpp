#include <gtest/gtest.h>

#include "auxiliary/socket/SecureSocket.hpp"




TEST(SecureSocketTest, IsMessageCiphered){

//    SecureSocket socket_server{std::make_unique<ZMQSocket>(zmq::socket_type::rep)};
//    SecureSocket socket_client{std::make_unique<ZMQSocket>(zmq::socket_type::req)};

    zmq::context_t context;
    zmq::socket_t socket{context, zmq::socket_type::push};

    auto data = socket.getsockopt<int>(ZMQ_TYPE);

    std::cout<<data<<" "<<ZMQ_PULL<<" "<<ZMQ_PUSH;
}