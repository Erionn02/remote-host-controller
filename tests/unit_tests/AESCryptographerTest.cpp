#include <gtest/gtest.h>

#include "auxiliary/AESCryptographer.hpp"

using namespace ::testing;

struct AESCryptographerTest : public Test{
    std::size_t data_length{1'000'000};

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



TEST_F(AESCryptographerTest, cipherDecipherTest){
    // given
    auto some_random_data = generateRandomData();
    AESCryptographer cryptographer{};

    //when
    auto ciphered_data = cryptographer.decipherData(some_random_data);
    auto deciphered_data = cryptographer.decipherData(ciphered_data);

    //then
    ASSERT_GE(ciphered_data.length(), some_random_data.length());
    ASSERT_EQ(deciphered_data, some_random_data);
}