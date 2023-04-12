#pragma once

#include "auxiliary/socket/ISocket.hpp"
#include "auxiliary/Runnable.hpp"


class RemoteHostConnectionController : public Runnable {
public:
    explicit RemoteHostConnectionController(std::unique_ptr<ISocket> command_socket,
                                            const std::string &command_socket_address,
                                            std::unique_ptr<ISocket> response_socket, const std::string &peers_address);

    std::string getBoundAddress();

private:
    void workerLoop() override;

    std::unique_ptr<ISocket> command_socket;
    std::unique_ptr<ISocket> response_socket;
};


