#include "ISocket.hpp"


class ZMQSocket: public ISocket{
public:
    explicit ZMQSocket(zmq::socket_type socket_type);

    void bind(const std::string &address) override;

    void connect(const std::string &address) override;

    void send(zmq::message_t message, zmq::send_flags flags = zmq::send_flags::none) override;

    void send(zmq::multipart_t message) override;

    zmq::recv_result_t recv(zmq::message_t &message) override;

    void recv(zmq::multipart_t &message) override;

    void setsockopt(int option, int option_value) override;

    zmq::socket_t socket;
    zmq::context_t context;
};