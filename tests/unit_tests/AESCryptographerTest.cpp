#include <gtest/gtest.h>

#include "auxiliary/AESCryptographer.hpp"

using namespace ::testing;

struct RSACryptographerTest : public Test{
    std::size_t data_length{1'000};

    std::string generateRandomData(){
        std::string some_random_data{};
        some_random_data.resize(data_length);
        auto generator = []{
            return std::rand()%255 +1;
        };

        std::generate_n(some_random_data.data(), data_length, generator);
        return some_random_data;
    }
};



TEST_F(RSACryptographerTest, oneCry){
    // given
    auto some_random_data = generateRandomData();
    AESCryptographer cryptographer{};

    //when
    auto ciphered_data = cryptographer.cipherData(some_random_data);
    auto deciphered_data = cryptographer.decipherData(ciphered_data);

    //then
    ASSERT_GE(ciphered_data.length(), some_random_data.length());
    ASSERT_EQ(deciphered_data, some_random_data);
}

TEST_F(RSACryptographerTest, oneCryptographerCanDecipherCipheredDataByOther){
    // given
    auto some_random_data = generateRandomData();
    AESCryptographer cryptographer{};
    AESCryptographer decryprographer{cryptographer.getKey(), cryptographer.getInitVector()};

    //when
    auto ciphered_data = cryptographer.cipherData(some_random_data);
    auto deciphered_data = decryprographer.decipherData(ciphered_data);

    //then
    ASSERT_GE(ciphered_data.length(), some_random_data.length());
    ASSERT_EQ(deciphered_data, some_random_data);
}

TEST_F(RSACryptographerTest, differentCryptographersWillGenerateDifferentCipheredData){
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