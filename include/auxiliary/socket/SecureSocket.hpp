#pragma once

#include "ZMQSocket.hpp"
#include "../RSACryptographer.hpp"
#include "../AESCryptographer.hpp"

#include <utility>

class SecureSocket : public ISocket{
public:
    SecureSocket(std::unique_ptr<ISocket> socket);

    void bind(const std::string &address) override;

    void connect(const std::string &address) override;

    void send(zmq::message_t &message, zmq::send_flags flags = zmq::send_flags::none) override;

    void send(zmq::multipart_t &message) override;

    zmq::recv_result_t recv(zmq::message_t &message) override;

    void recv(zmq::multipart_t &message) override;

    void setsockopt(int option, int option_value) override;

    int getsockopt(int option) override;

private:
    void exchangeKeys();
    void exchangeKeys(zmq::multipart_t &key);
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


