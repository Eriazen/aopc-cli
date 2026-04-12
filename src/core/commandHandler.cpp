#include "aopc-cli/core/commandHandler.hpp"
#include<iostream>
#include <sstream>
#include "aopc-cli/commands/helpCommand.hpp"
#include "aopc-cli/commands/exitCommand.hpp"
#include "aopc-cli/commands/invalidCommand.hpp"
#include "aopc-cli/commands/priceCommand.hpp"
#include "aopc-cli/commands/configCommand.hpp"


// Initialize the command map with available commands, associating command names with their respective command objects
void CommandHandler::initializeCommands() {
    m_commands["help"] = []() { return std::make_unique<HelpCommand>(); };
    m_commands["exit"] = []() { return std::make_unique<ExitCommand>(); };
    m_commands["price"] = []() { return std::make_unique<PriceCommand>(); };
    m_commands["config"] = []() { return std::make_unique<ConfigCommand>(); };
}

void CommandHandler::isoclineCompleter(ic_completion_env_t* cenv, const char* input) {
    ic_complete_word(cenv, input, &CommandHandler::isoclineWordCompleter, nullptr);
}

void CommandHandler::isoclineWordCompleter(ic_completion_env_t* cenv, const char* word) {
    void* arg = ic_completion_arg(cenv);
    if (arg == nullptr) return;

    CommandHandler* self = static_cast<CommandHandler*>(arg);
    self->handleCompletion(cenv, std::string(word));
}

void CommandHandler::handleCompletion(ic_completion_env_t* cenv, const std::string& word) {
    for (const auto& [cmd_name, cmd_logic] : m_commands) {
        if (cmd_name.find(word) == 0) {
            ic_add_completion(cenv, cmd_name.c_str());
        }
    }
}

// Main loop to continuously read user input, parse it, and execute the corresponding command
void CommandHandler::run() {
    ic_style_def("prompt_style", "ansi_blue");
    ic_set_default_completer(&CommandHandler::isoclineCompleter, this);

    while (true) {
        // Prompt user for input to execute a command
        char* rawInput;
        rawInput = ic_readline("[prompt-style] ");
        std::string userInput(rawInput);
        free(rawInput);

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
        auto it = m_commands.find(commandName);
        if (it != m_commands.end()) {
            std::unique_ptr<Command> newCommandInstance = it->second();
            newCommandInstance->execute(args);
        } else {
            std::make_unique<InvalidCommand>()->execute({});
        }
    }
}