#pragma once

#include "ISocket.hpp"
#include "../RSACryptographer.hpp"
#include "../AESCryptographer.hpp"

#include <utility>

class SecureSocket : public ISocket{
public:
    explicit SecureSocket(std::unique_ptr<ISocket> socket);
    explicit SecureSocket(zmq::socket_type type);

    void bind(const std::string &address) override;

    void unbind() override;

    void disconnect() override;

    void connect(const std::string &address) override;

    bool send(zmq::message_t &message, zmq::send_flags flags = zmq::send_flags::none) override;

    bool send(zmq::multipart_t &messages) override;

    bool recv(zmq::message_t &message) override;

    bool recv(zmq::multipart_t &messages) override;

    void setsockopt(int option, int option_value) override;

    int getsockopt(int option) override;

    void getsockopt(int option, void *value, size_t *value_size) override;
private:
    bool sendEncryptedAESKey();
    bool receiveAESKey();
    zmq::multipart_t serializePublicKey() const;
    CryptoPP::RSA::PublicKey deserializePublicKey(zmq::multipart_t& serialized_key) const;
    zmq::multipart_t encryptAESKey();
    std::pair<CryptoPP::SecByteBlock, CryptoPP::SecByteBlock> decryptAES(zmq::multipart_t& encrypted_aes_key);

    AESCryptographer aes{};
    RSACryptographer rsa{};

    bool exchanged_keys{false};
    std::unique_ptr<ISocket> socket;

    static inline std::string ACK{"ACK"};
};


