#pragma once

#include "aopc-cli/core/command.hpp"


class InvalidCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override {
            (void)args; // Avoid unused parameter warning
            std::cout << "Invalid command. Type 'help' for a list of available commands." << std::endl;
        }
};