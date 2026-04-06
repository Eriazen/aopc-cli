#include "aopc-cli/commands/helpCommand.hpp"
#include<iostream>


void HelpCommand::execute(const std::vector<std::string>& args) {
    if (!checkArgs(args, 0)) {
        return;
    }
    std::cout << "Available commands:\n" << std::endl;\
    std::cout << "      help - Show this help message." << std::endl;
    std::cout << "      exit - Exit the application." << std::endl;
    std::cout << "      price - Check the price of an item." << std::endl;
}