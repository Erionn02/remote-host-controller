#include <gtest/gtest.h>

#include "auxiliary/RSACryptographer.hpp"

using namespace ::testing;

struct RSACryptographerTest : public Test {
    std::size_t data_length{40};

    std::string generateRandomData() {
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

    //when
    auto encrypted_data = cryptographer.encrypt(data).value();
    auto decrypted_data = cryptographer.decrypt(encrypted_data);

    //then
    ASSERT_NE(RSACryptographer::KEY_SIZE, encrypted_data.size());
    ASSERT_NE(std::memcmp(encrypted_data.data(), data.data(), data.length()), 0);
    ASSERT_TRUE(decrypted_data.has_value());
    ASSERT_EQ(data.length(), decrypted_data.value().length());
    ASSERT_EQ(std::memcmp(data.data(), decrypted_data.value().data(), data.length()), 0);
}

TEST_F(RSACryptographerTest, twoDefaultCryptographersCannotDecipherEachOthersData) {
    //given
    RSACryptographer cryptographer_1{};
    RSACryptographer cryptographer_2{};
    std::string data = generateRandomData();

    //when
    auto encrypted_data_1 = cryptographer_1.encrypt(data);
    auto encrypted_data_2 = cryptographer_2.encrypt(data);
    auto decrypted_data_1 = cryptographer_1.decrypt(encrypted_data_2.value());
    auto decrypted_data_2 = cryptographer_2.decrypt(encrypted_data_1.value());

    //then
    ASSERT_FALSE(decrypted_data_1.has_value());
    ASSERT_FALSE(decrypted_data_2.has_value());
}

TEST_F(RSACryptographerTest, cannotEncryptTooLongData) {
    //given
    RSACryptographer cryptographer{};
    data_length = 10000;
    std::string data = generateRandomData();

    //when
    auto encrypted_data = cryptographer.encrypt(data);

    //then
    ASSERT_FALSE(encrypted_data.has_value());
}

TEST_F(RSACryptographerTest, cannotDecryptTooLongData) {
    //given
    RSACryptographer cryptographer{};
    data_length = 10000;
    std::string data = generateRandomData();

    //when
    auto decrypted_data = cryptographer.decrypt(data);

    //then
    ASSERT_FALSE(decrypted_data.has_value());
}

TEST_F(RSACryptographerTest, twoCryptographersAfterExchangingPublicKeysCanCommunicate) {
    //given
    RSACryptographer cryptographer_1{};
    RSACryptographer cryptographer_2{};
    auto cryptographer_1_public_key = cryptographer_1.getPublicKey();
    cryptographer_1.setPublicKey(cryptographer_2.getPublicKey());
    cryptographer_2.setPublicKey(cryptographer_1_public_key);

    // when & then
    for(int i=0;i<10;++i){
        std::string data = generateRandomData();

        auto encrypted_data_1 = cryptographer_1.encrypt(data);
        auto encrypted_data_2 = cryptographer_2.encrypt(data);

        auto decrypted_data_1 = cryptographer_1.decrypt(encrypted_data_2.value());
        auto decrypted_data_2 = cryptographer_2.decrypt(encrypted_data_1.value());

        ASSERT_EQ(data, decrypted_data_1);
        ASSERT_EQ(data, decrypted_data_2);
    }
}