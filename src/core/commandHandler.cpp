#include "aopc-cli/core/commandHandler.hpp"
#include<iostream>
#include <sstream>
#include "aopc-cli/commands/helpCommand.hpp"
#include "aopc-cli/commands/exitCommand.hpp"
#include "aopc-cli/commands/invalidCommand.hpp"
#include "aopc-cli/commands/priceCommand.hpp"


// Initialize the command map with available commands, associating command names with their respective command objects
void CommandHandler::initializeCommands() {
    commands["help"] = std::make_unique<HelpCommand>();
    commands["exit"] = std::make_unique<ExitCommand>();
    commands["invalid"] = std::make_unique<InvalidCommand>();
    commands["price"] = std::make_unique<PriceCommand>();
}

// Main loop to continuously read user input, parse it, and execute the corresponding command
void CommandHandler::run() {
    while (true) {
        // Prompt user for input to execute a command
        std::cout << "> ";
        std::getline(std::cin, userInput);

        // Skip empty input, prompting the user again
        if (userInput.empty()) continue;

        // Parse the user input into command and arguments using a string stream
        std::istringstream userInputStream(userInput);
        std::string arg;
        std::vector<std::string> args;

        // Extract the command and its arguments from the user input, storing them in a vector
        while (userInputStream >> arg) {
            args.push_back(arg);
        }

        // The first argument is the command name, and the rest are its arguments. Remove the command name from the arguments vector for easier processing.
        std::string commandName = args[0];
        args.erase(args.begin());

        // Find the command in the command map and execute it with the provided arguments.
        // If the command is not found, execute the "invalid" command to handle unrecognized commands.
        auto it = commands.find(commandName);
        if (it != commands.end()) {
            it->second->execute(args);
        } else {
            commands["invalid"]->execute({});
        }
    }
}