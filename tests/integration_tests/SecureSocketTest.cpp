#include <gtest/gtest.h>

#include "auxiliary/socket/SecureSocket.hpp"

#include <cryptopp/rsa.h>
#include <cryptopp/rng.h>
#include <cryptopp/randpool.h>
#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>


TEST(SecureSocketTest, IsMessageCiphered){

//    SecureSocket socket_server{std::make_unique<ZMQSocket>(zmq::socket_type::rep)};
//    SecureSocket socket_client{std::make_unique<ZMQSocket>(zmq::socket_type::req)};

    zmq::context_t context;
    zmq::socket_t socket{context, zmq::socket_type::push};

    auto data = socket.getsockopt<int>(ZMQ_TYPE);

    std::cout<<data<<" "<<ZMQ_REP<<" "<<ZMQ_REQ;

    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng,2048);
    const CryptoPP::Integer& n = params.GetModulus();
    const CryptoPP::Integer& p = params.GetPrime1();
    const CryptoPP::Integer& q = params.GetPrime2();
    const CryptoPP::Integer& d = params.GetPrivateExponent();
    const CryptoPP::Integer& e = params.GetPublicExponent();
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
    const int KEY_SIZE=16;
    CryptoPP::byte key[ KEY_SIZE ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, 0x00, KEY_SIZE );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

    //
    // String and Sink setup
    //
    std::string plaintext = "Now is the time for all good men to come to the aide...";
    std::string ciphertext;
    std::string decryptedtext;

    //
    // Dump Plain Text
    //
    std::cout << "Plain Text (" << plaintext.size() << " bytes)\n" << plaintext<<'\n';

    //
    // Create Cipher Text
    //
    CryptoPP::AES::Encryption aesEncryption(key, KEY_SIZE);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() );
    stfEncryptor.MessageEnd();

    //
    // Dump Cipher Text
    //
    std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

    for( int i = 0; i < ciphertext.size(); i++ ) {

        std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
    }

    std::cout << std::endl << std::endl;

    //
    // Decrypt
    //
    CryptoPP::AES::Decryption aesDecryption(key, KEY_SIZE);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
    stfDecryptor.MessageEnd();

    //
    // Dump Decrypted Text
    //
    std::cout << "Decrypted Text: " << std::endl;
    std::cout << decryptedtext;
    std::cout << std::endl << std::endl;



}


TEST(SecureSocketTest, testingLimitationsOfZMQ){
    srand(time(nullptr));

    ZMQSocket client{zmq::socket_type::req};
    ZMQSocket server{zmq::socket_type::rep};
    std::string bind_address = "tcp://127.0.0.1:2137";

    server.bind(bind_address);
    client.connect(bind_address);
    std::size_t data_size = 1'000'000;
    auto data = std::make_unique<unsigned char[]>(data_size);
    std::generate_n(data.get(),data_size,generator);

    zmq::message_t message{data.get(), data_size};

    client.send(std::move(message));

    zmq::message_t received{1};
    server.recv(received);


    ASSERT_EQ(received.size(), data_size);
    ASSERT_EQ(std::memcmp(received.data(), data.get(), data_size),0);
}