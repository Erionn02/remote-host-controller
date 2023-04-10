#pragma once

#include "auxiliary/socket/ISocket.hpp"
#include "auxiliary/Runnable.hpp"

#include "auxiliary/THIRD_PARTY/pstream.hpp"

class RemoteHostConnectionController : public Runnable {
public:
    explicit RemoteHostConnectionController(std::unique_ptr<ISocket> socket, const std::string &address_without_port);

    std::string getBoundAddress();

private:
    void workerLoop() override;

    std::unique_ptr<ISocket> socket;
};


