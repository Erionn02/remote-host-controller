#include "auxiliary/AESCryptographer.hpp"

#include <spdlog/spdlog.h>
#include <optional>

CryptoPP::SecByteBlock generateRandom(std::size_t size) {
    CryptoPP::SecByteBlock byte_block{size};
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(byte_block.data(), byte_block.size());
    return byte_block;
}


AESCryptographer::AESCryptographer() : key(generateRandom(CryptoPP::AES::DEFAULT_KEYLENGTH)),
                                       initialization_vector(generateRandom(CryptoPP::AES::BLOCKSIZE)),
                                       encryptor(key, key.size(), initialization_vector),
                                       decryptor(key, key.size(), initialization_vector)
                                       {

}

AESCryptographer::AESCryptographer(const CryptoPP::SecByteBlock &key, const CryptoPP::SecByteBlock &iv) : key(key),
                                                                                                          initialization_vector(iv),
                                                                                                          encryptor(key, key.size(), initialization_vector),
                                                                                                          decryptor(key, key.size(), initialization_vector)
                                                                                                         {}


std::string AESCryptographer::cipherData(const std::string &data) {
    return cipherData(data.data(), data.length());
}

std::string AESCryptographer::cipherData(const void *data_ptr, std::size_t size) {
    std::string ciphered_data{};

    encryptor.SetKeyWithIV(key, key.size(), initialization_vector); //for some reason this needs to be set over again, otherwise does not work
    CryptoPP::StringSource s(reinterpret_cast<const CryptoPP::byte*>(data_ptr), size, true,
                   new CryptoPP::StreamTransformationFilter(encryptor,
                                                  new CryptoPP::StringSink(ciphered_data)
                   )); // CryptoPP takes care of deallocation, so no worries

    return ciphered_data;
}

std::optional<std::string> AESCryptographer::decipherData(const std::string &data) {
    return decipherData(data.data(), data.length());
}

std::optional<std::string> AESCryptographer::decipherData(const void *data_ptr, std::size_t size) {
    std::optional<std::string> deciphered_data{std::string{}};
    try{
        decryptor.SetKeyWithIV(key, key.size(), initialization_vector); //for some reason this needs to be set over again, otherwise does not work
        CryptoPP::StringSource s(reinterpret_cast<const CryptoPP::byte*>(data_ptr), size, true,
                                 new CryptoPP::StreamTransformationFilter(decryptor,
                                                                          new CryptoPP::StringSink(deciphered_data.value())
                                 )); // CryptoPP takes care of deallocation, so no worries
    }
    catch(CryptoPP::Exception& e){
        spdlog::warn(e.what());
        return std::nullopt;
    }

    return deciphered_data;
}

const CryptoPP::SecByteBlock & AESCryptographer::getKey() {
    return key;
}

const CryptoPP::SecByteBlock & AESCryptographer::getInitVector() {
    return initialization_vector;
}

void AESCryptographer::setKeyAndInitVector(CryptoPP::SecByteBlock &new_key, CryptoPP::SecByteBlock &new_init_vector) {
    key = new_key;
    initialization_vector =  new_init_vector;
    decryptor.SetKeyWithIV(key, key.size(), initialization_vector);
    encryptor.SetKeyWithIV(key, key.size(), initialization_vector);
}

void AESCryptographer::setKey(CryptoPP::SecByteBlock &new_key) {
    setKeyAndInitVector(new_key, initialization_vector);
}

void AESCryptographer::setInitVec(CryptoPP::SecByteBlock &new_init_vec) {
    setKeyAndInitVector(key, new_init_vec);
}

