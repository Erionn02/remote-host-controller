#include <gtest/gtest.h>

#include "auxiliary/socket/ZMQSocket.hpp"

// I won't test member methods except the static ones that I added, because I would just test the lib at this point.
// But I still need the wrapper

TEST(ZMQSocketTest, isAddressValidTest) {
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp://123.123.123.123:66000")); // port too big
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("ftcp://123.123.123.123:1234")); // protocol incorrect
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp://123.256.123.123:1234")); // one octet too big
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp//123.123.123.123:1234")); // missing first colon
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp://123.123.123.1231234")); // missing second colon
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp://123.123.123:1234")); // one octet missing
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("123.123.123.123:1234")); // missing protocol
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("some random string")); // missing first colon
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp://123.123.123.123:-1234")); // port negative lol
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp:/123.123.123.123:1234")); // missing one slash
    ASSERT_FALSE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp:123.123.123.123:1234")); // missing two slashes

    ASSERT_TRUE(ZMQSocket::isAddressWithPortAndProtocolValid("tcp://123.123.123.123:1234")); // correct TCP address
}

TEST(ZMQSocketTest, getAvailableBindAddressTest) {
    auto tcp_bind_address = ZMQSocket::getAvailableBindAddress("127.0.0.1");
    ASSERT_TRUE(ZMQSocket::isAddressWithPortAndProtocolValid(tcp_bind_address));
}

