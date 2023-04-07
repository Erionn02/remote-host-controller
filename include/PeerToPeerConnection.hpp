#pragma once

#include "auxiliary/socket/ISocket.hpp"
#include "auxiliary/Runnable.hpp"

class PeerToPeerConnection : public Runnable {
public:
    explicit PeerToPeerConnection(std::unique_ptr<ISocket> socket, const std::string &address_without_port);

    std::size_t getBoundPort();

private:
    void workerLoop() override;

    std::unique_ptr<ISocket> socket;
};


