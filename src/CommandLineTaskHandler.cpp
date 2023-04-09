#include "CommandLineTaskHandler.hpp"

nlohmann::json CommandLineTaskHandler::createTask(const std::string &command) {
    nlohmann::json task{};

    task["type_of_task"] = "command";
    task["value"] = command;

    return task;
}
