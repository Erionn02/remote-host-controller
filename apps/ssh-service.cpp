#include <iostream>
#include <string>
#include <memory>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <chrono>
#include <unordered_map>

#include "ClientService.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/socket/ZMQSocket.hpp"

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::size_t MY_SSH_DEFAULT_PORT{2222};

int main(int argc, char *argv[]) {
    if (argc == 1) {
        auto underlying_socket = std::make_unique<ZMQSocket>(zmq::socket_type::rep);
        auto secure_socket = std::make_unique<SecureSocket>(std::move(underlying_socket));
        ClientService service{std::move(secure_socket), "127.0.0.1", MY_SSH_DEFAULT_PORT};
        service.start();
    } else if (argc == 2) {
        std::string address = argv[1];

    }
    std::cout << exec("ls -al") << '\n';
    std::cout << exec("pwd") << '\n';
    std::cout << exec("echo xdddd") << '\n';
    std::cout << exec("wefgwefwef") << '\n';
    std::cout << exec("nano file.txt") << '\n';

    return 0;
}
