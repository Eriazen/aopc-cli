#pragma once

#include "aopc-cli/core/command.hpp"
#include "aopc-cli/io/formatter.hpp"
#include <iostream>


class ClearCommand : public ICommand {
    public:
        std::unique_ptr<ICommand> create() const override { return std::make_unique<ClearCommand>(); };
        void execute(const std::vector<std::string>& args) override {
            (void) args;
            std::cout << "\x1b[1;1H\x1b[2J" << "\x1b[1;1H\x1b[3J";
            TextFormatter::printIntroMessage();
        }
};