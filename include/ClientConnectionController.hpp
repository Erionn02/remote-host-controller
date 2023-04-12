#pragma once

#include "auxiliary/Runnable.hpp"
#include "auxiliary/socket/ISocket.hpp"
#include "CommandLineTaskHandler.hpp"

class ClientConnectionController : public Runnable{
public:
    ClientConnectionController(std::unique_ptr<ISocket> command_socket, const std::string &peer_address,
                               std::unique_ptr<ISocket> response_socket, const std::string &bind_address);

private:
    void workerLoop() override;

    std::unique_ptr<ISocket> command_socket;
    std::unique_ptr<ISocket> data_socket;
};


