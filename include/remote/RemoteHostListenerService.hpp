#pragma once

#include "auxiliary/socket/ISocket.hpp"
#include "auxiliary/Runnable.hpp"
#include "remote/RemoteHostConnectionController.hpp"

#include <vector>

class RemoteHostListenerService : public Runnable {
public:
    RemoteHostListenerService(std::unique_ptr<ISocket> socket, const std::string &bind_address, std::size_t port);

private:
    void workerLoop() override;
    void removeStoppedConnections();
    bool isLimitOfConnectionsReached();

    std::unique_ptr<ISocket> socket;
    std::string address;
    std::vector<std::unique_ptr<RemoteHostConnectionController>> connections{};
    std::size_t limit_of_connections{10};
};


