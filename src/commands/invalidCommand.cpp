#include "aopc-cli/commands/invalidCommand.hpp"
#include<iostream>


void InvalidCommand::execute(const std::vector<std::string>& args) {
    std::cout << "Invalid command. Type 'help' for a list of available commands." << std::endl;
}