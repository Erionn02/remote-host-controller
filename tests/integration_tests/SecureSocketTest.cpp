#include <gtest/gtest.h>

#include "auxiliary/socket/SecureSocket.hpp"

#include <cryptopp/rsa.h>
#include <cryptopp/rng.h>
#include <cryptopp/randpool.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>



TEST(SecureSocketTest, IsMessageCiphered) {
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 2048);
    const CryptoPP::Integer &n = params.GetModulus();
    const CryptoPP::Integer &p = params.GetPrime1();
    const CryptoPP::Integer &q = params.GetPrime2();
    const CryptoPP::Integer &d = params.GetPrivateExponent();
    const CryptoPP::Integer &e = params.GetPublicExponent();
    using namespace std;


    cout << "RSA Parameters:" << endl;
    cout << " n: " << n << endl;
    cout << " p: " << p << endl;
    cout << " q: " << q << endl;
    cout << " d: " << d << endl;
    cout << " e: " << e << endl;
    cout << endl;

    //Key and IV setup
    //AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-
    //bit). This key is secretly exchanged between two parties before communication
    //begins. DEFAULT_KEYLENGTH= 16 bytes
    const int KEY_SIZE = 16;
    CryptoPP::byte key[KEY_SIZE], iv[CryptoPP::AES::BLOCKSIZE];
    memset(key, 0x00, KEY_SIZE);
    memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);

    //
    // String and Sink setup
    //
    std::string plaintext;
    std::size_t size = 10'000'000;
    plaintext.resize(size);
    auto generator = []() {
        return std::rand() % 255 + 1;
    };
    std::generate_n(plaintext.data(), size, generator);
    std::string ciphertext;
    std::string decryptedtext;

    //
    // Dump Plain Text
    //

    //
    // Create Cipher Text
    //
    CryptoPP::AES::Encryption aesEncryption(key, KEY_SIZE);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char *>( plaintext.c_str()), plaintext.length());
    stfEncryptor.MessageEnd();

    //
    // Dump Cipher Text
    //
//    std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;
//
//    for( int i = 0; i < ciphertext.size(); i++ ) {
//
//        std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
//    }
//
//    std::cout << std::endl << std::endl;

    //
    // Decrypt
    //
    CryptoPP::AES::Decryption aesDecryption(key, KEY_SIZE);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
    stfDecryptor.Put(reinterpret_cast<const unsigned char *>( ciphertext.c_str()), ciphertext.size());
    stfDecryptor.MessageEnd();

    std::cout<<decryptedtext.length();
    ASSERT_EQ(plaintext.length(), decryptedtext.length());
    ASSERT_EQ(plaintext, decryptedtext);
}
