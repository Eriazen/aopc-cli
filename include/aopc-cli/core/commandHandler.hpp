#pragma once

#include <map>
#include <memory>
#include "aopc-cli/core/command.h"


// Command handler class
class CommandHandler {
    public:
        CommandHandler();
        void run();

    private:
        std::string userInput;
        std::map<std::string, std::unique_ptr<Command>> commands;

        void initializeCommands();
};