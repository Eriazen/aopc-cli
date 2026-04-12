#pragma once

#include "aopc-cli/core/command.hpp"
#include "aopc-cli/core/settings.hpp"
#include <functional>


class ConfigCommand : public ICommand {
    public:
        ConfigCommand();

        std::unique_ptr<ICommand> create() const override { return std::make_unique<ConfigCommand>(); };
        void execute(const std::vector<std::string>& args) override;

    private:
        std::unordered_map<std::string, std::function<void(const std::string&)>> m_setters;
};