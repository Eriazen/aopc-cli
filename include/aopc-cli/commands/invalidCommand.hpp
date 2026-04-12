#pragma once

#include "aopc-cli/core/command.hpp"


class InvalidCommand : public ICommand {
    public:
        std::unique_ptr<ICommand> create() const override { return std::make_unique<InvalidCommand>(); };
        void execute(const std::vector<std::string>& args) override {
            (void)args; // Avoid unused parameter warning
            std::cout << "Invalid command. Type 'help' for a list of available commands." << std::endl;
        }
};