#include "auxiliary/RSACryptographer.hpp"

#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <fmt/format.h>


RSACryptographer::RSACryptographer() {
    CryptoPP::InvertibleRSAFunction params{};
    params.Initialize(rng, RSACryptographer::KEY_SIZE);

    private_key.AssignFrom(params);
    public_key.AssignFrom(params);
    encryptor = CryptoPP::RSAES_OAEP_SHA_Encryptor(public_key);
    decryptor = CryptoPP::RSAES_OAEP_SHA_Decryptor(private_key);
}

RSACryptographer::RSACryptographer(CryptoPP::InvertibleRSAFunction &params) : private_key(params), public_key(params),
                                                                              encryptor(public_key),
                                                                              decryptor(private_key) {
}

RSACryptographer::RSACryptographer(const CryptoPP::RSA::PrivateKey &private_key,
                                   const CryptoPP::RSA::PublicKey &public_key) : private_key(private_key),
                                                                                 public_key(public_key),
                                                                                 encryptor(public_key),
                                                                                 decryptor(private_key) {

}

std::string RSACryptographer::encrypt(const std::string &data) {
    return encrypt(reinterpret_cast<const void*>(data.data()), data.size());
}


std::string RSACryptographer::encrypt(const void *data, std::size_t size) {
    std::string encrypted{};
    CryptoPP::StringSource(reinterpret_cast<const CryptoPP::byte*>(data),size, true,
                           new CryptoPP::PK_EncryptorFilter(rng,encryptor, new CryptoPP::StringSink(encrypted)));
    return encrypted;
}

std::string RSACryptographer::decrypt(const std::string &data) {
    return decrypt(reinterpret_cast<const void*>(data.data()), data.size());
}



std::string RSACryptographer::decrypt(const void *data, std::size_t size) {
    std::string decrypted_data{};

    CryptoPP::StringSource(reinterpret_cast<const CryptoPP::byte*>(data),size, true,
                           new CryptoPP::PK_DecryptorFilter(rng,decryptor, new CryptoPP::StringSink(decrypted_data)));

    return decrypted_data;
}



