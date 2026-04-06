#pragma once

#include "aopc-cli/core/command.h"


class InvalidCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override;
};