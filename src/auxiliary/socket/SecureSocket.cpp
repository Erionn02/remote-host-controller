#include "auxiliary/socket/SecureSocket.hpp"

#include <iostream>


SecureSocket::SecureSocket(std::unique_ptr<ISocket> socket) : socket(std::move(socket)) {
    int socket_type = this->socket->getsockopt(ZMQ_TYPE);
    if (socket_type != ZMQ_REP && socket_type != ZMQ_REQ) {
        throw std::runtime_error("For SecureSocket the only available types of underlying socket"
                                 " are zmq::socket_type::rep and zmq::socket_type::req.");
    }
}

void SecureSocket::bind(const std::string &address) {
    socket->bind(address);
}

void SecureSocket::connect(const std::string &address) {
    socket->connect(address);
}

void SecureSocket::send(zmq::message_t &message, zmq::send_flags flags) {
    if (!exchanged_keys) {
        exchangeKeys();
    }

    auto encrypted_data = aes.cipherData(message.data(), message.size());
    zmq::message_t encrypted_message{encrypted_data};
    socket->send(encrypted_message, flags);
}

void SecureSocket::send(zmq::multipart_t &message) {
    if (!exchanged_keys) {
        exchangeKeys();
    }
    zmq::multipart_t encrypted_messages{};
    for (auto &msg: message) {
        auto encrypted_data = aes.cipherData(msg.data(), msg.size());
        zmq::message_t encrypted_message{encrypted_data};
        encrypted_messages.add(std::move(encrypted_message));
    }

    socket->send(encrypted_messages);
}

zmq::recv_result_t SecureSocket::recv(zmq::message_t &message) {
    if (!exchanged_keys) {
        zmq::multipart_t key_message{};
        exchangeKeys(key_message);
    }

    auto result = socket->recv(message);
    auto msg_str = message.to_string();
    std::cout<<message.to_string();
    auto deciphered = aes.decipherData(message.data(), message.size());
    zmq::message_t tmp{deciphered};
    message.swap(tmp);
    return result;
}

void SecureSocket::recv(zmq::multipart_t &message) {
    return socket->recv(message);
}

void SecureSocket::setsockopt(int option, int option_value) {
    socket->setsockopt(option, option_value);
}

int SecureSocket::getsockopt(int option) {
    return socket->getsockopt(option);
}

void SecureSocket::exchangeKeys() {
    zmq::multipart_t key_message = serializePublicKey();
    socket->send(key_message);

    zmq::multipart_t peer_key_message{};
    socket->recv(peer_key_message);
    auto peer_public_key = deserializePublicKey(peer_key_message);
    rsa.setPublicKey(peer_public_key);

    auto encrypted_aes_key = encryptAESKey();

    socket->send(encrypted_aes_key);

    zmq::message_t ack{};
    socket->recv(ack);
    auto lol = aes.decipherData(ack.data(), ack.size());

    exchanged_keys = true;
    // assert(ack.to_string() == SecureSocket::ACK);
}


void SecureSocket::exchangeKeys(zmq::multipart_t &key) {
    socket->recv(key);
    auto peer_public_key = deserializePublicKey(key);
    auto my_public_key = serializePublicKey();
    rsa.setPublicKey(peer_public_key);
    socket->send(my_public_key);

    zmq::multipart_t encrypted_aes_key{};
    socket->recv(encrypted_aes_key);
    auto [aes_key, aes_vector] = decryptAES(encrypted_aes_key);
    auto wtf = AESCryptographer(aes_key, aes_vector);
    aes = wtf;
    auto ciphered_ack = aes.cipherData(SecureSocket::ACK);
    zmq::message_t ack{ciphered_ack};
    socket->send(ack);
    exchanged_keys = true;
}

zmq::multipart_t SecureSocket::serializePublicKey() const {
    auto &key = rsa.getPublicKey();
    auto &modulus = key.GetModulus();
    auto &public_exponent = key.GetPublicExponent();
    std::stringstream converted_key_stream{};
    converted_key_stream << modulus;
    zmq::message_t modulus_message{converted_key_stream.str()};
    converted_key_stream.str("");
    converted_key_stream.clear();
    converted_key_stream << public_exponent;
    zmq::message_t public_exponent_message{converted_key_stream.str()};

    zmq::multipart_t key_message{};
    key_message.add(std::move(modulus_message));
    key_message.add(std::move(public_exponent_message));
    return key_message;
}

CryptoPP::RSA::PublicKey SecureSocket::deserializePublicKey(zmq::multipart_t &serialized_key) const {
    auto &modulus_message = serialized_key.at(0);
    auto &public_exponent_message = serialized_key.at(1);

    CryptoPP::Integer modulus{modulus_message.data<char>()};
    CryptoPP::Integer public_exponent{public_exponent_message.data<char>()};

    CryptoPP::RSA::PublicKey public_key{};
    public_key.Initialize(modulus, public_exponent);
    return public_key;
}

zmq::multipart_t SecureSocket::encryptAESKey() {
    auto& aes_key = aes.getKey();
    auto& aes_init_vector = aes.getInitVector();

    auto encrypted_aes_key = rsa.encrypt(aes_key.data(), aes_key.size());
    auto encrypted_aes_init_vector = rsa.encrypt(aes_init_vector.data(), aes_init_vector.size());

    assert(encrypted_aes_key.has_value());
    assert(encrypted_aes_init_vector.has_value());

    zmq::message_t key_msg{encrypted_aes_key.value().data(), encrypted_aes_key.value().size()};
    zmq::message_t vector_msg{encrypted_aes_init_vector.value().data(), encrypted_aes_init_vector.value().size()};

    zmq::multipart_t encrypted_aes_message{};
    encrypted_aes_message.add(std::move(key_msg));
    encrypted_aes_message.add(std::move(vector_msg));
    return encrypted_aes_message;
}

std::pair<CryptoPP::SecByteBlock, CryptoPP::SecByteBlock> SecureSocket::decryptAES(zmq::multipart_t& encrypted_aes_key) {
    auto& encrypted_key = encrypted_aes_key.at(0);
    auto& encrypted_init_vector = encrypted_aes_key.at(1);

    auto key = rsa.decrypt(encrypted_key.data(), encrypted_key.size());
    auto init_vector = rsa.decrypt(encrypted_init_vector.data(), encrypted_init_vector.size());

    assert(key.has_value());
    assert(init_vector.has_value());

    CryptoPP::SecByteBlock aes_key {reinterpret_cast<CryptoPP::byte*>(key.value().data()), key.value().size()};
    CryptoPP::SecByteBlock aes_init_vector {reinterpret_cast<CryptoPP::byte*>(init_vector.value().data()),
                                            init_vector.value().size()};

    return {aes_key, aes_init_vector};
}

