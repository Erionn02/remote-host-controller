#pragma once

#include "auxiliary/Runnable.hpp"
#include "auxiliary/socket/SecureSocket.hpp"
#include "remote/ShellHandler.hpp"

class ClientConnectionController : public Runnable{
public:
    ClientConnectionController(std::unique_ptr<SecureSocket> command_socket, const std::string &peer_address,
                               std::unique_ptr<SecureSocket> response_socket, const std::string &bind_address);

private:
    void startUpHook() override;
    void stopHook() override;

    void workerLoop() override;
    void receiveWorkerLoop();

    inline std::ostream& stream(const std::string& stream_type);

    std::unique_ptr<SecureSocket> command_socket;
    std::unique_ptr<SecureSocket> data_socket;
    std::jthread receive_data_thread{};
};


