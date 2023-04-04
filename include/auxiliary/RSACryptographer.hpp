#pragma once
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>


class RSACryptographer {
public:
    RSACryptographer();
    RSACryptographer(CryptoPP::InvertibleRSAFunction& params);
    RSACryptographer(const CryptoPP::RSA::PrivateKey& private_key, const CryptoPP::RSA::PublicKey& public_key);

    std::string encrypt(const std::string& data);
    std::string encrypt(const void* data, std::size_t size);
    
    std::string decrypt(const std::string& data);
    std::string decrypt(const void* data, std::size_t size);

    static inline unsigned int KEY_SIZE{3072};
private:
    CryptoPP::AutoSeededRandomPool rng{};
    CryptoPP::RSA::PrivateKey private_key{};
    CryptoPP::RSA::PublicKey public_key{};

    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor;
    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor;
};


