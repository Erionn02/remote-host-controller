#pragma once

#include "auxiliary/socket/ISocket.hpp"

#include <nlohmann/json.hpp>
#include <zmq.hpp>

#include <future>


class TaskScheduler {
public:
    TaskScheduler(std::unique_ptr<ISocket> client, const std::string& address);

    std::future<zmq::message_t>& scheduleTask(const nlohmann::json& task);

private:
    std::unique_ptr<ISocket> client;
    std::future<zmq::message_t> future_task;
};


