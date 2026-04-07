#pragma once

#include "aopc-cli/core/command.hpp"


class InvalidCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override;
};