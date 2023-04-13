#pragma once

#include "auxiliary/socket/SecureSocket.hpp"

class ServiceConnector {
public:
    static std::string getRemoteHostAddress(const std::string& my_address, const std::string& remote_host_service_address, std::size_t port);
};


