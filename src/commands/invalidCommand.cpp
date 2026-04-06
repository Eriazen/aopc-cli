#include "aopc-cli/commands/invalidCommand.hpp"
#include<iostream>


void InvalidCommand::execute(const std::vector<std::string>& args) {
    (void)args; // Avoid unused parameter warning
    std::cout << "Invalid command. Type 'help' for a list of available commands." << std::endl;
}