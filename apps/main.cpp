#include <iostream>
#include <string>
#include <memory>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <chrono>

#include "auxiliary/Runnable.hpp"


class XDDDDD : public Runnable{
public:
    XDDDDD(){
        name = []{return "John Pablo II";};
        tear_down = [&]{myMethod();};
        set_up_hook = [] {spdlog::info("Ave Papau");};
    }

    void workerLoop() override{

    }
private:
    void myMethod(){
        spdlog::info(elo);
    }
    std::string elo{"no siema"};
};

int main() {
    std::unique_ptr<XDDDDD> xd= std::make_unique<XDDDDD>();

    xd->start();
    sleep(1);
    xd->stop();

    return 0;
}
