#include <gtest/gtest.h>

#include "auxiliary/RSACryptographer.hpp"

using namespace ::testing;

struct RSACryptographerTest : public Test {
    std::size_t data_length{40};

    std::string generateRandomData() {
        // sanity check
        assert(RSACryptographer::KEY_SIZE >= data_length);
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
    RSACryptographer cryptographer{};
    std::string data = generateRandomData();
    std::cout<<data.length();
    //when
    auto encrypted_data = cryptographer.encrypt(data);
    auto decrypted_data = cryptographer.decrypt(encrypted_data);

    //then
    ASSERT_NE(RSACryptographer::KEY_SIZE, encrypted_data.size());
    ASSERT_NE(std::memcmp(encrypted_data.data(), data.data(), data.length()), 0);
    ASSERT_EQ(data.length(), decrypted_data.length());
    ASSERT_EQ(std::memcmp(data.data(), decrypted_data.data(), data.length()), 0);
}

TEST_F(RSACryptographerTest, twoDefaultCryptographersCannotDecipherEachOthersData) {
    //given
    RSACryptographer cryptographer_1{};
    RSACryptographer cryptographer_2{};
    std::string data = generateRandomData();
    std::string data_cp{data};
    std::cout<<data.length();


    //when
    auto encrypted_data_1 = cryptographer_1.encrypt(data);
    auto encrypted_data_2 = cryptographer_2.encrypt(data_cp);
    auto decrypted_data_1 = cryptographer_1.decrypt(encrypted_data_2);
    auto decrypted_data_2 = cryptographer_2.decrypt(encrypted_data_1);

    //then
    ASSERT_NE(encrypted_data_1, encrypted_data_2);
    ASSERT_NE(decrypted_data_1, decrypted_data_2);
    ASSERT_NE(decrypted_data_1, data);
    ASSERT_NE(data, decrypted_data_2);
}