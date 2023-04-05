#include "TaskScheduler.hpp"


TaskScheduler::TaskScheduler(std::unique_ptr<ISocket> client, const std::string &address) : client(std::move(client)) {

    this->client->connect(address);
}

std::future<zmq::message_t> &TaskScheduler::scheduleTask(const nlohmann::json &task) {
    zmq::message_t message{task.dump()};

    client->send(message);


    future_task = std::async(std::launch::async, [this] {
        zmq::message_t response;
        client->recv(response);
        return response;
    });

    return future_task;
}
