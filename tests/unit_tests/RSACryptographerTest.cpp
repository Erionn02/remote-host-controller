#include <gtest/gtest.h>

#include "auxiliary/RSACryptographer.hpp"

using namespace ::testing;

struct RSACryptographerTest : public Test {
    std::size_t data_length{1'000};

    std::string generateRandomData() {
        // sanity check
        assert(RSACryptographer::KEY_SIZE > data_length);
        std::string some_random_data{};
        some_random_data.resize(data_length);
        auto generator = [] {
            return std::rand() % 255 + 1;
        };

        std::generate_n(some_random_data.data(), data_length, generator);
        return some_random_data;
    }

};

TEST_F(RSACryptographerTest, canDecipherCipheredData) {
    //given
    RSACryptographer cryptographer;
    std::string data = generateRandomData();

    //when
    auto encrypted_data = cryptographer.encrypt(data);
    auto decrypted_data = cryptographer.decrypt(encrypted_data);

    //then
    ASSERT_EQ(data.length(), encrypted_data.size());
    ASSERT_NE(std::memcmp(encrypted_data.data(), data.data(), data.length()), 0);
    ASSERT_EQ(data.length(), decrypted_data.size());
    ASSERT_EQ(std::memcmp(data.data(), decrypted_data.data(), data.length()), 0);
}