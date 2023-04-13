#pragma once

#include <string>

class CommandLineTaskHandler {
public:
    void executeCommand(const std::string& command);

    std::string getCommandOutput();

};


