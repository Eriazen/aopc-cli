#include "aopc-cli/core/commandHandler.hpp"
#include<iostream>
#include <sstream>
#include "aopc-cli/commands/helpCommand.hpp"
#include "aopc-cli/commands/exitCommand.hpp"
#include "aopc-cli/commands/invalidCommand.hpp"
#include "aopc-cli/commands/priceCommand.hpp"


// CommandHandler implementation
CommandHandler::CommandHandler() {
    initializeCommands();
}

// Initialize the command map with available commands
void CommandHandler::initializeCommands() {
    commands["help"] = std::make_unique<HelpCommand>();
    commands["exit"] = std::make_unique<ExitCommand>();
    commands["invalid"] = std::make_unique<InvalidCommand>();
    commands["price"] = std::make_unique<PriceCommand>();
}

void CommandHandler::run() {
    while (true) {
        // Prompt user for input
        std::cout << "> ";
        std::getline(std::cin, userInput);

        // Skip empty input
        if (userInput.empty()) {
            continue;
        }

        // Parse user input into command and arguments
        std::istringstream userInputStream(userInput);
        std::string arg;
        std::vector<std::string> args;
        while (userInputStream >> arg) {
            args.push_back(arg);
        }
        std::string commandName = args[0];
        args.erase(args.begin()); // Remove command name from arguments

        // Find and execute the command
        auto it = commands.find(commandName);
        if (it != commands.end()) {
            it->second->execute(args);
        } else {
            commands["invalid"]->execute({});
        }
    }
}