#include <gtest/gtest.h>

#include "auxiliary/AESCryptographer.hpp"

using namespace ::testing;

struct AESCryptographerTest : public Test {
    std::size_t data_length{10'000};

    std::string generateRandomData() {
        srand(time(nullptr));
        std::string some_random_data{};
        some_random_data.resize(data_length);
        auto generator = [] {
            return std::rand() % 255 + 1;
        };

        std::generate_n(some_random_data.data(), data_length, generator);
        return some_random_data;
    }
};


TEST_F(AESCryptographerTest, canDecipherCipheredData) {
    // given
    std::string some_random_data = generateRandomData();
    AESCryptographer cryptographer{};

    //when
    auto ciphered_data = cryptographer.cipherData(some_random_data);
    auto deciphered_data = cryptographer.decipherData(ciphered_data);

    //then
    ASSERT_GE(ciphered_data.length(), some_random_data.length());
    ASSERT_EQ(deciphered_data, some_random_data);
}

TEST_F(AESCryptographerTest, canDecipherDataMultipleTimes) {
    // given
    AESCryptographer cryptographer{};

    for (int i = 0; i < 10; ++i) {
        auto some_random_data = generateRandomData();

        //when
        auto ciphered_data = cryptographer.cipherData(some_random_data);
        auto deciphered_data = cryptographer.decipherData(ciphered_data);

        //then
        ASSERT_EQ(deciphered_data, some_random_data);
    }

}

TEST_F(AESCryptographerTest, oneCryptographerCanDecipherCipheredDataByOther) {
    // given
    auto some_random_data = generateRandomData();
    AESCryptographer cryptographer{};
    AESCryptographer decryptographer{cryptographer.getKey(), cryptographer.getInitVector()};

    //when
    auto ciphered_data = cryptographer.cipherData(some_random_data);
    auto deciphered_data = decryptographer.decipherData(ciphered_data);

    //then
    ASSERT_GE(ciphered_data.length(), some_random_data.length());
    ASSERT_EQ(deciphered_data, some_random_data);
}

TEST_F(AESCryptographerTest, differentCryptographersWillGenerateDifferentCipheredData) {
    // given
    auto some_random_data = generateRandomData();
    AESCryptographer some_cryptographer{};
    AESCryptographer some_other_cryptographer{};

    //when
    auto ciphered_data_first = some_cryptographer.cipherData(some_random_data);
    auto ciphered_data_second = some_other_cryptographer.cipherData(some_random_data);


    //then
    ASSERT_NE(std::memcmp(ciphered_data_first.data(), ciphered_data_second.data(), data_length), 0);
}