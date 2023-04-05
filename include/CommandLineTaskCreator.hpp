#pragma once
#include <nlohmann/json.hpp>

class CommandLineTaskCreator {
public:
    nlohmann::json createTask(const std::string& command);
};


