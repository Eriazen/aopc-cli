#pragma once

#include "aopc-cli/core/command.hpp"
#include <iostream>


class ClearCommand : public ICommand {
    public:
        std::unique_ptr<ICommand> create() const override { return std::make_unique<ClearCommand>(); };
        void execute(const std::vector<std::string>& args) override {
            (void) args;
            std::cout << "\x1b[1;1H\x1b[2J" << "\x1b[1;1H\x1b[3J"
                << constants::C_HL1 << "\x1b[4mA\x1b[24m" << constants::C_TEXT << "lbion "
                << constants::C_HL1 << "\x1b[4mO\x1b[24m" << constants::C_TEXT << "nline "
                << constants::C_HL1 << "\x1b[4mP\x1b[24m" << constants::C_TEXT << "rofit "
                << constants::C_HL1 << "\x1b[4mC\x1b[24m" << constants::C_TEXT << "alculator CLI tool started!\n"

                << "Type '" << constants::C_HL1 << "help" << constants::C_TEXT 
                << "' for a list of available commands.\n\n";
        }
};