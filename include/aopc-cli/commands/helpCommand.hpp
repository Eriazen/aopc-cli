#pragma once

#include "aopc-cli/core/command.hpp"
#include <unordered_map>
#include <functional>


class HelpCommand : public ICommand {
    public:
        HelpCommand();

        std::unique_ptr<ICommand> create() const override { return std::make_unique<HelpCommand>(); };
        void execute(const std::vector<std::string>& args) override;
        void complete(ic_completion_env_t* cenv, const std::string& word, const std::string& line) override;

    private:
        std::unordered_map<std::string, std::function<void()>> m_commands;

        void printGeneralHelp() const;
        void printPriceHelp() const;
        void printConfigHelp() const;
};