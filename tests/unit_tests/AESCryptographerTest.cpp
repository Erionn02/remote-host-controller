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

    // when
    auto ciphered_data = cryptographer.cipherData(some_random_data);
    auto deciphered_data = cryptographer.decipherData(ciphered_data);

    // then
    ASSERT_GE(ciphered_data.length(), some_random_data.length());
    ASSERT_EQ(deciphered_data, some_random_data);
}

TEST_F(AESCryptographerTest, canHandleEmptyData) {
    // given
    std::string empty_data {};
    AESCryptographer cryptographer{};

    // when
    auto ciphered_data = cryptographer.cipherData(empty_data);
    auto deciphered_data = cryptographer.decipherData(ciphered_data);

    // then
    ASSERT_GE(ciphered_data.length(), empty_data.length());
    ASSERT_EQ(deciphered_data, empty_data);
}

TEST_F(AESCryptographerTest, canDecipherDataMultipleTimes) {
    // given
    AESCryptographer cryptographer{};

    for (int i = 0; i < 10; ++i) {
        auto some_random_data = generateRandomData();

        // when
        auto ciphered_data = cryptographer.cipherData(some_random_data);
        auto deciphered_data = cryptographer.decipherData(ciphered_data);

        // then
        ASSERT_EQ(deciphered_data, some_random_data);
    }

}

TEST_F(AESCryptographerTest, oneCryptographerCanDecipherCipheredDataByOther) {
    // given
    std::string some_random_data = "elo elo 320 co tam mordy";
    AESCryptographer cryptographer_1{};
    AESCryptographer cryptographer_2{cryptographer_1.getKey(), cryptographer_1.getInitVector()};

    // when
    auto ciphered_data_1 = cryptographer_1.cipherData(some_random_data.data(), some_random_data.size());
    auto ciphered_data_2 = cryptographer_2.cipherData(some_random_data.data(), some_random_data.size());


    // then
    // That test might seem like overkill, but believe me, it is not. It caught bug.
    ASSERT_EQ(ciphered_data_1, ciphered_data_2);
    auto deciphered_1 = cryptographer_1.decipherData(ciphered_data_2);
    auto deciphered_1_again = cryptographer_1.decipherData(ciphered_data_2);
    ASSERT_EQ(deciphered_1, some_random_data);
    ASSERT_EQ(deciphered_1_again, some_random_data);
    ASSERT_EQ(cryptographer_1.decipherData(ciphered_data_2), cryptographer_1.decipherData(ciphered_data_2));
    ASSERT_EQ(cryptographer_2.decipherData(ciphered_data_1), cryptographer_2.decipherData(ciphered_data_2));
    ASSERT_EQ(cryptographer_1.decipherData(ciphered_data_1), cryptographer_2.decipherData(ciphered_data_2));
    ASSERT_EQ(cryptographer_1.decipherData(ciphered_data_2), cryptographer_2.decipherData(ciphered_data_1));
}

TEST_F(AESCryptographerTest, differentCryptographersWillGenerateDifferentCipheredData) {
    // given
    auto some_random_data = generateRandomData();
    AESCryptographer some_cryptographer{};
    AESCryptographer some_other_cryptographer{};

    // when
    auto ciphered_data_first = some_cryptographer.cipherData(some_random_data);
    auto ciphered_data_second = some_other_cryptographer.cipherData(some_random_data);


    // then
    ASSERT_NE(std::memcmp(ciphered_data_first.data(), ciphered_data_second.data(), data_length), 0);
}