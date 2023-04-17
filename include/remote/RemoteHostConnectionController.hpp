#pragma once

#include "auxiliary/socket/SecureSocket.hpp"
#include "auxiliary/Runnable.hpp"
#include "ShellHandler.hpp"


// Command socket is `pull` type of socket, and response socket is `push`
// I decided not to go with rep/req pattern because responses could be deferred like a lot (because of inputs, to programs
// that are running in shell, etc.), therefore loose type-of-communication is more preferred in this

class RemoteHostConnectionController : public Runnable {
public:
    explicit RemoteHostConnectionController(std::unique_ptr<SecureSocket> command_socket,
                                            const std::string &command_socket_address,
                                            std::unique_ptr<SecureSocket> response_socket, const std::string &peers_address);

    std::string getBoundAddress();

private:
    void startUpHook() override;
    void stopHook() override;

    void commandOutputWorkerLoop();
    void workerLoop() override;

    std::unique_ptr<SecureSocket> command_socket;
    std::unique_ptr<SecureSocket> response_socket;
    ShellHandler shell;
    std::jthread response_thread;
};


