#pragma once

#include "auxiliary/socket/ISocket.hpp"
#include "auxiliary/Runnable.hpp"
#include "PeerToPeerConnection.hpp"

#include <vector>

class ClientService : public Runnable {
public:
    ClientService(std::unique_ptr<ISocket> socket, const std::string &bind_address, std::size_t port);

private:
    void workerLoop() override;
    bool isNewConnectionRequestReceived();
    void removeStoppedConnections();

    std::unique_ptr<ISocket> socket;
    std::string address;
    std::vector<std::unique_ptr<PeerToPeerConnection>> connections{};
};


