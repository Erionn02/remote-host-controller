#pragma once

#include <cryptopp/rsa.h>
#include <cryptopp/rng.h>
#include <cryptopp/randpool.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>


class AESCryptographer {
public:
    explicit AESCryptographer();
    explicit AESCryptographer(const CryptoPP::SecByteBlock& key, const CryptoPP::SecByteBlock & iv);

    std::string cipherData(const std::string& data);
    std::string cipherData(const void* data_ptr, std::size_t size);

    std::string decipherData(const std::string& data);
    std::string decipherData(const void* data_ptr, std::size_t size);

    const CryptoPP::SecByteBlock& getKey();
    const CryptoPP::SecByteBlock& getInitVector();
private:
    CryptoPP::SecByteBlock key{};
    CryptoPP::SecByteBlock initialization_vector{};

    CryptoPP::AES::Encryption aes_encryption;
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption;

    CryptoPP::AES::Decryption aes_decryption;
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption;
};


