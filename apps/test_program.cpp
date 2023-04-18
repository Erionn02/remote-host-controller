#include <iostream>
#include <chrono>
#include <thread>


using namespace std::chrono_literals;

int main() {
    std::cout<<"Started program.";
    while(true){
        std::cout<<"\nEnter input: ";
        std::string lol{};

        std::getline(std::cin, lol);
        std::cout<<'\b';
    }
    return 0;
}