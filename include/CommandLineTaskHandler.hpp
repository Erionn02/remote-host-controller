#pragma once
#include <nlohmann/json.hpp>

class CommandLineTaskHandler {
public:


    nlohmann::json createTask(const std::string& command);
};


