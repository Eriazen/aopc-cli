#include "aopc-cli/io/cmdParser.hpp"
#include<iostream>


void HelpCommand::execute() {
    std::cout << "Available commands:\n" << std::endl;
}

void ExitCommand::execute() {
    std::cout << "Exiting..." << std::endl;
    exit(0);
}

void InvalidCommand::execute() {
    std::cout << "Invalid command. Type 'help' for a list of available commands." << std::endl;
}

CommandHandler::CommandHandler() {
    initializeCommands();
}

void CommandHandler::initializeCommands() {
    commands["help"] = std::make_unique<HelpCommand>();
    commands["exit"] = std::make_unique<ExitCommand>();
    commands["invalid"] = std::make_unique<InvalidCommand>();
}

void CommandHandler::run() {
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput.empty()) {
            continue;
        }

        auto it = commands.find(userInput);

        if (it != commands.end()) {
            it->second->execute();
        }
        else {
            commands["invalid"]->execute();
        }
    }
}