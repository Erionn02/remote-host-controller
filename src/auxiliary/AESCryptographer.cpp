#include "auxiliary/AESCryptographer.hpp"

template<typename T>
AESCryptographer::KeyType generateRandom(){
    T data{};
    srand(static_cast<unsigned int>(time(nullptr)));
    auto generator = []{return std::rand()%256;};
    std::generate_n(data.data(), data.size(),generator);
    return data;
}


AESCryptographer::AESCryptographer() : key(generateRandom<KeyType>()),
                                       initialization_vector(generateRandom<InitType>()),
                                       aes_encryption(key.data(), key.size()),
                                       cbc_encryption(aes_encryption, initialization_vector.data()),
                                       aes_decryption(key.data(), KEY_SIZE),
                                       cbc_decryption(aes_decryption, initialization_vector.data()) {
}

AESCryptographer::AESCryptographer(const AESCryptographer::KeyType &key, const AESCryptographer::InitType &iv) : key(key),
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
                                                                                                             KEY_SIZE),
                                                                                                     cbc_decryption(
                                                                                                             aes_decryption,
                                                                                                             initialization_vector.data()) {}


std::string AESCryptographer::cipherData(const std::string &data) {
    return cipherData(data.data(), data.length());
}

std::string AESCryptographer::cipherData(const void *data_ptr, std::size_t size) {
    std::string ciphered_data{};

    CryptoPP::StreamTransformationFilter stfEncryptor(cbc_encryption, new CryptoPP::StringSink(ciphered_data));
    stfEncryptor.Put(reinterpret_cast<const unsigned char *>( data_ptr), size);
    stfEncryptor.MessageEnd();

    return ciphered_data;
}

std::string AESCryptographer::decipherData(const std::string &data) {
    return decipherData(data.data(), data.length());
}

std::string AESCryptographer::decipherData(const void *data_ptr, std::size_t size) {
    std::string deciphered_data{};

    CryptoPP::StreamTransformationFilter stf_decryptor(cbc_decryption, new CryptoPP::StringSink(deciphered_data));
    stf_decryptor.Put(reinterpret_cast<const unsigned char *>( data_ptr), size);
    stf_decryptor.MessageEnd();

    return deciphered_data;
}

const AESCryptographer::KeyType &AESCryptographer::getKey() {
    return key;
}

const AESCryptographer::InitType &AESCryptographer::getInitVector() {
    return initialization_vector;
}

