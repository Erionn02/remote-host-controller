#include <iostream>
#include <string>
#include <memory>
#include <zmq.hpp>




int main() {
    auto xd1 = "elo";
    auto xd2 = "elo""elo";
    std::cout<<xd2;

    zmq::context_t cont{};
    zmq::socket_t s{cont, zmq::socket_type::push};
    zmq::message_t m{"eki"};
    s.connect("tcp://127.0.0.1:12345");
    s.send(m, zmq::send_flags::none);
    return 0;
}
