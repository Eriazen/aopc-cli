#include "aopc-cli/commands/helpCommand.hpp"
#include<iostream>


void HelpCommand::execute(const std::vector<std::string>& args) {
    if (!checkArgs(args, 0)) {
        return;
    }
    std::cout << "Available commands:\n" << std::endl;
}