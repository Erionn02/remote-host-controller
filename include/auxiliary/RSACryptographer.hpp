#pragma once
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>


class RSACryptographer {
public:
    RSACryptographer();
    RSACryptographer(CryptoPP::InvertibleRSAFunction& params);
    RSACryptographer(const CryptoPP::RSA::PrivateKey& private_key, const CryptoPP::RSA::PublicKey& public_key);

    CryptoPP::SecByteBlock encrypt(const std::string& data);
    CryptoPP::SecByteBlock encrypt(const void* data, std::size_t size);
    CryptoPP::SecByteBlock encrypt(const CryptoPP::SecByteBlock& data);
    
    
    CryptoPP::SecByteBlock decrypt(const std::string& data);
    CryptoPP::SecByteBlock decrypt(const void* data, std::size_t size);
    CryptoPP::SecByteBlock decrypt(const CryptoPP::SecByteBlock& data);

    static inline unsigned int KEY_SIZE{2048};
private:
    CryptoPP::AutoSeededRandomPool rng{};
    CryptoPP::RSA::PrivateKey private_key;
    CryptoPP::RSA::PublicKey public_key;

    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor;
    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor;
};


