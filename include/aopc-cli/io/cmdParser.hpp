#pragma once

#include <map>
#include <string>
#include <memory>
#include "aopc-cli/io/natsManager.hpp"

class Command {
    public:
        virtual void execute() = 0;
        virtual ~Command() = default;
};

class HelpCommand : public Command {
    public:
        void execute() override;
};

class ExitCommand : public Command {
    public:
        void execute() override;
};

class InvalidCommand : public Command {
    public:
        void execute() override;
};

class PriceCommand : public Command {
    public:
        void execute() override;
};

class CommandHandler {
    public:
        CommandHandler();
        void run();

    private:
        std::string userInput;
        std::map<std::string, std::unique_ptr<Command>> commands;

        void initializeCommands();
};