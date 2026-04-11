#pragma once

#include "aopc-cli/core/command.hpp"


class HelpCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override;

    private:
        void printGeneralHelp() const;
        void printPriceHelp() const;
        void printConfigHelp() const;
};