#include "auxiliary/RSACryptographer.hpp"

#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <fmt/format.h>


//RSACryptographer::RSACryptographer() {
//    CryptoPP::InvertibleRSAFunction params{};
//    params.GenerateRandomWithKeySize(rng, RSACryptographer::KEY_SIZE);
//    private_key = CryptoPP::RSA::PrivateKey(params);
//    CryptoPP::RSA::PublicKey lmao{params};
//    public_key = CryptoPP::RSA::PublicKey(params);
//    encryptor = CryptoPP::RSAES_OAEP_SHA_Encryptor(public_key);
//    decryptor = CryptoPP::RSAES_OAEP_SHA_Decryptor(private_key);
//}

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

CryptoPP::SecByteBlock RSACryptographer::encrypt(const std::string &data) {
    return encrypt(reinterpret_cast<const void*>(data.data()), data.length());
}

CryptoPP::SecByteBlock RSACryptographer::encrypt(const CryptoPP::SecByteBlock &data) {
    return encrypt(data.data(), data.size());
}

CryptoPP::SecByteBlock RSACryptographer::encrypt(const void *data, std::size_t size) {
    if (size > encryptor.FixedMaxPlaintextLength()) {
        throw std::runtime_error(fmt::format("You can't encrypt data longer than key size ({}>{})",
                                             size, encryptor.FixedMaxPlaintextLength()));
    }

    std::size_t ecl = encryptor.CiphertextLength(size);
    CryptoPP::SecByteBlock ciphered_text{ecl};
    encryptor.Encrypt(rng, reinterpret_cast<const CryptoPP::byte*>(data), size, ciphered_text);

    return ciphered_text;
}

CryptoPP::SecByteBlock RSACryptographer::decrypt(const std::string &data) {
    return decrypt(reinterpret_cast<const void*>(data.data()), data.length());
}

CryptoPP::SecByteBlock RSACryptographer::decrypt(const CryptoPP::SecByteBlock &data) {
    return decrypt(data.data(), data.size());
}


CryptoPP::SecByteBlock RSACryptographer::decrypt(const void *data, std::size_t size) {
    if (size > decryptor.FixedMaxPlaintextLength()) {
        throw std::runtime_error(fmt::format("You can't decrypt data longer than key size ({}>{})",
                                             size, decryptor.FixedMaxPlaintextLength()));
    }

    size_t dpl = decryptor.MaxPlaintextLength( size );
    CryptoPP::SecByteBlock deciphered_data{dpl};
    decryptor.Decrypt(rng, reinterpret_cast<const CryptoPP::byte*>(data), size, deciphered_data);
    return deciphered_data;
}



