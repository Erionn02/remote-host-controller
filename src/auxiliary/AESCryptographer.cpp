#include "auxiliary/AESCryptographer.hpp"


CryptoPP::SecByteBlock generateRandom(std::size_t size){
    CryptoPP::SecByteBlock byte_block{size};
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(byte_block.data(), byte_block.size());
    return byte_block;
}


AESCryptographer::AESCryptographer() : key(generateRandom(CryptoPP::AES::DEFAULT_KEYLENGTH)),
                                       initialization_vector(generateRandom(CryptoPP::AES::BLOCKSIZE)),
                                       aes_encryption(key.data(), key.size()),
                                       cbc_encryption(aes_encryption, initialization_vector.data()),
                                       aes_decryption(key.data(), key.size()),
                                       cbc_decryption(aes_decryption, initialization_vector.data()) {
}

AESCryptographer::AESCryptographer(const CryptoPP::SecByteBlock &key, const CryptoPP::SecByteBlock &iv) : key(key),
                                                                                                     initialization_vector(
                                                                                                             iv),
                                                                                                     aes_encryption(
                                                                                                             key.data(),
                                                                                                             key.size()),
                                                                                                     cbc_encryption(
                                                                                                             aes_encryption,
                                                                                                             initialization_vector.data()),
                                                                                                     aes_decryption(
                                                                                                             key.data(),
                                                                                                             key.size()),
                                                                                                     cbc_decryption(
                                                                                                             aes_decryption,
                                                                                                             initialization_vector.data()) {}


std::string AESCryptographer::cipherData(const std::string &data) {
    return cipherData(data.data(), data.length());
}

std::string AESCryptographer::cipherData(const void *data_ptr, std::size_t size) {
    std::string ciphered_data{};

    CryptoPP::StreamTransformationFilter stfEncryptor(cbc_encryption, new CryptoPP::StringSink(ciphered_data));
    stfEncryptor.Put(reinterpret_cast<const CryptoPP::byte*>(data_ptr), size);
    stfEncryptor.MessageEnd();

    return ciphered_data;
}

std::string AESCryptographer::decipherData(const std::string &data) {
    return decipherData(data.data(), data.length());
}

std::string AESCryptographer::decipherData(const void *data_ptr, std::size_t size) {
    std::string deciphered_data{};

    CryptoPP::StreamTransformationFilter stf_decryptor(cbc_decryption, new CryptoPP::StringSink(deciphered_data));
    stf_decryptor.Put(reinterpret_cast<const CryptoPP::byte*>(data_ptr), size);
    stf_decryptor.MessageEnd();

    return deciphered_data;
}

const CryptoPP::SecByteBlock &AESCryptographer::getKey() {
    return key;
}

const CryptoPP::SecByteBlock &AESCryptographer::getInitVector() {
    return initialization_vector;
}

