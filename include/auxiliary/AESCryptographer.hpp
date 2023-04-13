#pragma once

#include <cryptopp/rsa.h>
#include <cryptopp/rng.h>
#include <cryptopp/randpool.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>

#include <optional>


class AESCryptographer {
public:
    explicit AESCryptographer();
    explicit AESCryptographer(const CryptoPP::SecByteBlock& key, const CryptoPP::SecByteBlock & iv);

    void setKeyAndInitVector(CryptoPP::SecByteBlock& new_key, CryptoPP::SecByteBlock& new_init_vector);
    void setKey(CryptoPP::SecByteBlock& new_key);
    void setInitVec(CryptoPP::SecByteBlock& new_init_vec);

    std::string cipherData(const std::string& data);
    std::string cipherData(const void* data_ptr, std::size_t size);

    std::optional<std::string> decipherData(const std::string& data);
    std::optional<std::string> decipherData(const void* data_ptr, std::size_t size);

    const CryptoPP::SecByteBlock & getKey();
    const CryptoPP::SecByteBlock & getInitVector();
private:
    CryptoPP::SecByteBlock key{};
    CryptoPP::SecByteBlock initialization_vector{};

    CryptoPP::CBC_Mode< CryptoPP::AES >::Encryption encryptor;
    CryptoPP::CBC_Mode< CryptoPP::AES >::Decryption decryptor;
};


