#include "PeerFactory.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/AESCryptographer.hpp"

std::unique_ptr<ClientConnectionController>
PeerFactory::createClientConnectionController(const std::string &remote_address, const std::string &bind_address) {
    auto command_socket = std::make_unique<SecureSocket>(zmq::socket_type::req);
    std::size_t size{16};
    auto key_buffer = std::make_unique<unsigned char[]>(size);
    auto init_vec_buffer = std::make_unique<unsigned char[]>(size);
    command_socket->getsockopt(AES_KEY, key_buffer.get(), &size);
    command_socket->getsockopt(AES_VEC, init_vec_buffer.get(), &size);

    CryptoPP::SecByteBlock AES_key{key_buffer.get(), size};
    CryptoPP::SecByteBlock AES_init_vector{init_vec_buffer.get(), size};

    auto response_socket = std::make_unique<SecureSocket>(zmq::socket_type::pull, AES_key,
                                                          AES_init_vector);


    return std::make_unique<ClientConnectionController>(std::move(command_socket), remote_address,
                                                        std::move(response_socket), bind_address);
}


std::unique_ptr<RemoteHostConnectionController>
PeerFactory::createRemoteHostConnectionController(const std::string &bind_address, const std::string &client_address) {
    auto command_socket = std::make_unique<SecureSocket>(zmq::socket_type::rep);
    std::size_t size{16};
    auto key_buffer = std::make_unique<unsigned char[]>(size);
    auto init_vec_buffer = std::make_unique<unsigned char[]>(size);
    command_socket->getsockopt(AES_KEY, key_buffer.get(), &size);
    command_socket->getsockopt(AES_VEC, init_vec_buffer.get(), &size);

    CryptoPP::SecByteBlock AES_key{key_buffer.get(), size};
    CryptoPP::SecByteBlock AES_init_vector{init_vec_buffer.get(), size};

    auto response_socket = std::make_unique<SecureSocket>(zmq::socket_type::push, AES_key,
                                                          AES_init_vector);



    return std::make_unique<RemoteHostConnectionController>(std::move(command_socket), bind_address,
                                                            std::move(response_socket), client_address);
}
