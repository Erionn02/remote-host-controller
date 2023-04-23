#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

#include <spdlog/spdlog.h>

#include <iostream>

#define RETURN_ON_FAILURE(operation) \
if(!(operation)) return false;

SecureSocket::SecureSocket(std::unique_ptr<ISocket> socket) : socket(std::move(socket)) {
    int socket_type = this->socket->getsockopt(ZMQ_TYPE);
    if (socket_type != ZMQ_REP && socket_type != ZMQ_REQ) {
        throw std::runtime_error("For SecureSocket the only available types of underlying socket in this constructor"
                                 " are zmq::socket_type::rep and zmq::socket_type::req.");
    }
}

SecureSocket::SecureSocket(zmq::socket_type type) : SecureSocket(std::make_unique<ZMQSocket>(type)) {}

SecureSocket::SecureSocket(zmq::socket_type type, const CryptoPP::SecByteBlock &key,
                           const CryptoPP::SecByteBlock &initialization_vector) :
        socket(std::make_unique<ZMQSocket>(type)), aes(key, initialization_vector) {
    exchanged_keys = true;
}


void SecureSocket::bind(const std::string &address) {
    socket->bind(address);
}

void SecureSocket::unbind() {
    socket->unbind();
}

void SecureSocket::connect(const std::string &address) {
    socket->connect(address);
}

void SecureSocket::disconnect() {
    socket->disconnect();
}

void SecureSocket::setsockopt(int option, void *value, size_t value_size) {
    if (option == AES_KEY) {
        CryptoPP::SecByteBlock key{reinterpret_cast<CryptoPP::byte *>(value), value_size};
        aes.setKey(key);
    } else if (option == AES_VEC) {
        CryptoPP::SecByteBlock init_vec{reinterpret_cast<CryptoPP::byte *>(value), value_size};
        aes.setInitVec(init_vec);
    }
}


void SecureSocket::setsockopt(int option, int option_value) {
    if (option == EXCHANGED_KEYS) {
        exchanged_keys = option_value != 0;
    } else {
        socket->setsockopt(option, option_value);
    }
}

int SecureSocket::getsockopt(int option) {
    return socket->getsockopt(option);
}

std::string SecureSocket::getLastEndpoint() {
    return socket->getLastEndpoint();
}


void SecureSocket::getsockopt(int option, void *value, size_t *value_size) {
    if (option == AES_KEY) {
        auto &key = aes.getKey();
        if (*value_size < key.size()) {
            throw std::runtime_error("Size of given ptr is too small.");
        }
        std::memcpy(value, key.data(), key.size());
    } else if (option == AES_VEC) {
        auto &vec = aes.getInitVector();
        if (*value_size < vec.size()) {
            throw std::runtime_error("Size of given ptr is too small.");
        }
        std::memcpy(value, vec.data(), vec.size());
    } else {
        socket->getsockopt(option, value, value_size);
    }
}

bool SecureSocket::send(zmq::message_t &message, zmq::send_flags flags) {
    if (!exchanged_keys) {
        if (!sendEncryptedAESKey()) {
            exchanged_keys = false;
            return exchanged_keys;
        }
        spdlog::debug("Sent AES key successfully.");
    }
    auto encrypted_data = aes.cipherData(message.data(), message.size());
    zmq::message_t encrypted_message{encrypted_data};
    return socket->send(encrypted_message, flags);
}

bool SecureSocket::send(zmq::multipart_t &messages) {
    if (!exchanged_keys) {
        if (!sendEncryptedAESKey()) {
            exchanged_keys = false;
            return exchanged_keys;
        }
        spdlog::debug("Sent AES key successfully.");
    }
    zmq::multipart_t encrypted_messages{};
    for (auto &msg: messages) {
        auto encrypted_data = aes.cipherData(msg.data(), msg.size());
        zmq::message_t encrypted_message{encrypted_data};
        encrypted_messages.add(std::move(encrypted_message));
    }

    return socket->send(encrypted_messages);
}

bool SecureSocket::recv(zmq::message_t &message) {
    if (!exchanged_keys) {
        if (!receiveAESKey()) {
            exchanged_keys = false;
            return exchanged_keys;
        }
        spdlog::debug("Received AES key.");
    }

    auto recv_result = socket->recv(message);
    if (recv_result) {
        auto msg_str = message.to_string();
        auto deciphered = aes.decipherData(message.data(), message.size());
        if (deciphered.has_value()) {
            zmq::message_t tmp{deciphered.value()};
            message.swap(tmp);
        } else {
            // means that received data is not ciphered or is ciphered with different key than used in this command_socket,
            // probably someone is ears dropping
            zmq::message_t nack{"NACK"};
            socket->send(nack);
            return false;
        }
    }
    return recv_result;
}

bool SecureSocket::recv(zmq::multipart_t &messages) {
    if (!exchanged_keys) {
        if (!receiveAESKey()) {
            exchanged_keys = false;
            return exchanged_keys;
        }
        spdlog::debug("Received AES key.");
    }
    auto recv_result = socket->recv(messages);
    if (recv_result) {
        for (auto &single_message: messages) {
            auto deciphered = aes.decipherData(single_message.data(), single_message.size());
            if (deciphered.has_value()) {
                zmq::message_t tmp{deciphered.value()};
                single_message.swap(tmp);
            } else {
                // means that received data is not ciphered or is ciphered with different key than used in this command_socket,
                // probably someone is ears dropping
                zmq::message_t nack{"NACK"};
                socket->send(nack);
                return false;
            }
        }
    }

    return recv_result;
}

bool SecureSocket::sendEncryptedAESKey() {
    zmq::multipart_t rsa_key_message = serializePublicKey();
    RETURN_ON_FAILURE(socket->send(rsa_key_message))

    zmq::multipart_t peer_rsa_key_message{};
    RETURN_ON_FAILURE(socket->recv(peer_rsa_key_message))

    auto peer_public_key = deserializePublicKey(peer_rsa_key_message);
    rsa.setPublicKey(peer_public_key);
    auto encrypted_aes_key = encryptAESKey();
    RETURN_ON_FAILURE(socket->send(encrypted_aes_key))

    exchanged_keys = true;
    zmq::message_t ack{};
    recv(ack);
    return ack.to_string() == SecureSocket::ACK;
}


bool SecureSocket::receiveAESKey() {
    zmq::multipart_t rsa_key{};
    RETURN_ON_FAILURE(socket->recv(rsa_key));

    auto peer_public_key = deserializePublicKey(rsa_key);
    auto my_public_key = serializePublicKey();
//    rsa.setPublicKey(peer_public_key);
    RETURN_ON_FAILURE(socket->send(my_public_key));

    zmq::multipart_t encrypted_aes_key{};
    RETURN_ON_FAILURE(socket->recv(encrypted_aes_key));
    auto [aes_key, aes_vector] = decryptAES(encrypted_aes_key);
    aes.setKeyAndInitVector(aes_key, aes_vector);

    exchanged_keys = true;
    zmq::message_t ack{SecureSocket::ACK};
    return send(ack);
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
    auto &aes_key = aes.getKey();
    auto &aes_init_vector = aes.getInitVector();

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

std::pair<CryptoPP::SecByteBlock, CryptoPP::SecByteBlock>
SecureSocket::decryptAES(zmq::multipart_t &encrypted_aes_key) {
    auto &encrypted_key = encrypted_aes_key.at(0);
    auto &encrypted_init_vector = encrypted_aes_key.at(1);

    auto key = rsa.decrypt(encrypted_key.data(), encrypted_key.size());
    auto init_vector = rsa.decrypt(encrypted_init_vector.data(), encrypted_init_vector.size());

    assert(key.has_value());
    assert(init_vector.has_value());

    CryptoPP::SecByteBlock aes_key{reinterpret_cast<CryptoPP::byte *>(key.value().data()), key.value().size()};
    CryptoPP::SecByteBlock aes_init_vector{reinterpret_cast<CryptoPP::byte *>(init_vector.value().data()),
                                           init_vector.value().size()};

    return {aes_key, aes_init_vector};
}


