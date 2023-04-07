#include "ISocket.hpp"


class ZMQSocket: public ISocket{
public:
    explicit ZMQSocket(zmq::socket_type socket_type);

    void bind(const std::string &address) override;

    void unbind() override;

    void connect(const std::string &address) override;

    void disconnect() override;

    bool send(zmq::message_t &message, zmq::send_flags flags = zmq::send_flags::none) override;

    bool send(zmq::multipart_t &message) override;

    int getsockopt(int option) override;

    void getsockopt(int option, void *value, size_t *value_size) override;

    bool recv(zmq::message_t &message) override;

    bool recv(zmq::multipart_t &message) override;

    void setsockopt(int option, int option_value) override;

private:
    zmq::context_t context{};
    zmq::socket_t socket{};
    std::string last_used_address{};
};