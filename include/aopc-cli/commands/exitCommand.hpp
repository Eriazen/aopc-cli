#pragma once

#include "aopc-cli/core/command.hpp"


class ExitCommand : public ICommand {
    public:
        std::unique_ptr<ICommand> create() const override { return std::make_unique<ExitCommand>(); };
        void execute(const std::vector<std::string>& args) override;
};