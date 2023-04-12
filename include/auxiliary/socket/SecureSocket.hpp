#pragma once

#include "ISocket.hpp"
#include "../RSACryptographer.hpp"
#include "../AESCryptographer.hpp"

#include <utility>

#define AES_KEY 2137
#define AES_VEC 420
#define EXCHANGED_KEYS 2137420


class SecureSocket : public ISocket{
public:
    explicit SecureSocket(std::unique_ptr<ISocket> socket);
    explicit SecureSocket(zmq::socket_type type);
    explicit SecureSocket(zmq::socket_type type, const CryptoPP::SecByteBlock& key, const CryptoPP::SecByteBlock& initialization_vector);

    void bind(const std::string &address) override;

    void unbind() override;

    void disconnect() override;

    void connect(const std::string &address) override;

    bool send(zmq::message_t &message, zmq::send_flags flags = zmq::send_flags::none) override;

    bool send(zmq::multipart_t &messages) override;

    bool recv(zmq::message_t &message) override;

    bool recv(zmq::multipart_t &messages) override;

    void setsockopt(int option, int option_value) override;
    void setsockopt(int option, void *value, size_t value_size);

    int getsockopt(int option) override;
    void getsockopt(int option, void *value, size_t *value_size) override;

    std::string getLastEndpoint() override;

    bool sendEncryptedAESKey();

    bool receiveAESKey();
private:
    zmq::multipart_t serializePublicKey() const;
    CryptoPP::RSA::PublicKey deserializePublicKey(zmq::multipart_t& serialized_key) const;
    zmq::multipart_t encryptAESKey();
    std::pair<CryptoPP::SecByteBlock, CryptoPP::SecByteBlock> decryptAES(zmq::multipart_t& encrypted_aes_key);


    bool exchanged_keys{false};
    std::unique_ptr<ISocket> socket;
    AESCryptographer aes{};
    RSACryptographer rsa{};

    static inline std::string ACK{"ACK"};
};


