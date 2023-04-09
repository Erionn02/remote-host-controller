#pragma once

#include "auxiliary/Runnable.hpp"
#include "auxiliary/socket/ISocket.hpp"
#include "CommandLineTaskHandler.hpp"

class ClientConnectionController : public Runnable{
public:
    ClientConnectionController(std::unique_ptr<ISocket> socket, const std::string &peer_address);

private:
    void workerLoop() override;

    std::unique_ptr<ISocket> socket;
};


