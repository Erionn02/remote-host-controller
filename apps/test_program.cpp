#include <iostream>
#include <chrono>
#include <thread>


using namespace std::chrono_literals;

int main() {
    std::cout<<"Started program.\n";
    while(true){
        std::cout<<"some input lmaoo\n";
        std::cerr<<"\nsomething in error stream\n";
        if(std::rand()%3==0){
            std::cout<<"\nEnter input: ";
            std::string lol{};

            std::getline(std::cin, lol);
            std::cout<<"Got input: "<<lol<<'\n';
            if(lol == "end"){
                std::cout<<"Terminating.";
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}