#include "ISocket.hpp"


class ZMQSocket: public ISocket{
public:
    explicit ZMQSocket(zmq::socket_type socket_type);

    void bind(const std::string &address) override;

    void connect(const std::string &address) override;

    void send(zmq::message_t message, zmq::send_flags flags) override;

    void send(zmq::multipart_t message) override;

    void recv(zmq::message_t message, zmq::recv_flags flags) override;

    void recv(zmq::multipart_t message) override;

    void setsockopt(int option) override;

    zmq::socket_t socket;
    zmq::context_t context;
};