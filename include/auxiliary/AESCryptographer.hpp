#pragma once

#include <cryptopp/rsa.h>
#include <cryptopp/rng.h>
#include <cryptopp/randpool.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>


class AESCryptographer {
public:
    static const inline std::size_t KEY_SIZE{16};
    using KeyType = std::array<CryptoPP::byte, AESCryptographer::KEY_SIZE>;
    using InitType = std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE>;

    explicit AESCryptographer();
    explicit AESCryptographer(const KeyType& key, const InitType & iv);

    std::string cipherData(const std::string& data);
    std::string cipherData(const void* data_ptr, std::size_t size);

    std::string decipherData(const std::string& data);
    std::string decipherData(const void* data_ptr, std::size_t size);

    const KeyType& getKey();
    const InitType& getInitVector();
private:
    KeyType key{};
    InitType initialization_vector{};

    CryptoPP::AES::Encryption aes_encryption;
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption;

    CryptoPP::AES::Decryption aes_decryption;
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption;
};


