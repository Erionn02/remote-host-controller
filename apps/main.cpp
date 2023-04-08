#include <iostream>
#include <string>
#include <memory>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <chrono>
#include <unordered_map>

std::string exec(const char* cmd) {
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


int main() {
    std::cout<<exec("ls -al")<<'\n';
    std::cout<<exec("pwd")<<'\n';
    std::cout<<exec("echo xdddd")<<'\n';
    std::cout<<exec("wefgwefwef")<<'\n';
    std::cout<<exec("nano file.txt")<<'\n';

    return 0;
}
