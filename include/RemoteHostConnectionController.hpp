#pragma once

#include "auxiliary/socket/ISocket.hpp"
#include "auxiliary/Runnable.hpp"


class RemoteHostConnectionController : public Runnable {
public:
    explicit RemoteHostConnectionController(std::unique_ptr<ISocket> command_socket, const std::string &address_without_port);

    std::string getBoundAddress();

private:
    void workerLoop() override;

    std::unique_ptr<ISocket> command_socket;
    std::unique_ptr<ISocket> response_socket;
};


