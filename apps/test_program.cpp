#include <iostream>
#include <chrono>
#include <thread>

#include "auxiliary/socket/ZMQSocket.hpp"

#include "auxiliary/portable/PortableGetChar.hpp"

using namespace std::chrono_literals;



int main() {
//    ZMQSocket socket{zmq::socket_type::pull};
//    socket.bind("tcp://127.0.0.1:2137");
//    std::cout<<"Starting listening...\n";
//    while(true){
//        zmq::message_t m{};
//        socket.recv(m);
//        std::cout<<m.to_string()<<'\n';
//    }

    std::cout<<"kuba@Desktop:~/CLionProjects/remote-host-controller$";
    while(true) {
        char c = portable::p_getChar();
        std::cout<<c;
//        std::cout.flush();
    }

//    std::cout<<"Started program.\n";
//    while(true){
//        std::cout<<"some input lmaoo\n";
//        std::cerr<<"\nsomething in error stream\n";
//        if(std::rand()%3==0){
//            std::cout<<"\nEnter input: ";
//            std::string lol{};
//
//            std::getline(std::cin, lol);
//            std::cout<<"Got input: "<<lol<<'\n';
//            if(lol == "end"){
//                std::cout<<"Terminating.";
//                break;
//            }
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//    }
    return 0;
}


//#include <termios.h>
//#include <stdio.h>
//#include <csignal>
//#include <cstring>
//#include <iostream>
//
//void get_password(char *password)
//{
//    static struct termios old_terminal;
//    static struct termios new_terminal;
//
//    //get settings of the actual terminal
//    tcgetattr(STDIN_FILENO, &old_terminal);
//
//    // do not echo the characters
//    new_terminal = old_terminal;
//    new_terminal.c_lflag &= static_cast<tcflag_t>(~(ECHO));
//
//    // set this as the new terminal options
//    tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal);
//
//    // get the password
//    // the user can add chars and delete if he puts it wrong
//    // the input process is done when he hits the enter
//    // the \n is stored, we replace it with \0
//    if (fgets(password, BUFSIZ, stdin) == NULL)
//        password[0] = '\0';
//    else
//        password[strlen(password)-1] = '\0';
//
//    // go back to the old settings
//    tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
//}
//
//int main(void)
//{
//    char password[BUFSIZ];
//
//    std::cout<<"Insert password:";
//    get_password(password);
//    std::cout<<password;
//}