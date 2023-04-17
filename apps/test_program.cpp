#include <iostream>
#include <chrono>
#include <thread>


using namespace std::chrono_literals;

int main() {
    std::cout<<"Started program.\n";
    while(true){
        std::cout<<"Something"<<std::endl;
        std::cerr<<"error stream"<<std::endl;
        std::this_thread::sleep_for(0.5s);
        if(std::rand()%3==0){
            std::cout<<"Waiting for input...\n";
            std::string input{};
            std::getline(std::cin, input);
            std::cout<<"Got input: \""<<input<<"\"\n";
            if(input=="end"){
                break;
            }
        }
    }
    return 0;
}