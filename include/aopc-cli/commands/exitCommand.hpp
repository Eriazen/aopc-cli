#pragma once

#include "aopc-cli/core/command.hpp"


class ExitCommand : public ICommand {
    public:
        void execute(const std::vector<std::string>& args) override;
};