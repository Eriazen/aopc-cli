#pragma once

#include <map>
#include <memory>
#include <functional>
#include "aopc-cli/core/command.hpp"


// Command handler class
class CommandHandler {
    public:
        CommandHandler() { initializeCommands(); };
        void run();

    private:
        std::unordered_map<std::string, std::function<std::unique_ptr<Command>()>> m_commands;

        void initializeCommands();
};