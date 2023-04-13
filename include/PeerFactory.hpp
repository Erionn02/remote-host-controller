#pragma once

#include "client/ClientConnectionController.hpp"
#include "remote/RemoteHostConnectionController.hpp"

class PeerFactory {
public:
    static std::unique_ptr<ClientConnectionController>
    createClientConnectionController(const std::string &remote_address, const std::string &bind_address);

    static std::unique_ptr<RemoteHostConnectionController>
    createRemoteHostConnectionController(const std::string &bind_address, const std::string &client_address);

};


