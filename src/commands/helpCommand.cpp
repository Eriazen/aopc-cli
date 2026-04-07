#include "aopc-cli/commands/helpCommand.hpp"
#include "aopc-cli/io/argParser.hpp"
#include<iostream>


void HelpCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);
    // Check if the number of arguments is correct
    if (!parser.checkArgs(0)) return;
    
    // Print the help message with available commands
    std::cout << "Available commands:\n" << std::endl;\
    std::cout << "      help - Show this help message." << std::endl;
    std::cout << "      exit - Exit the application." << std::endl;
    std::cout << "      price - Check the price of an item." << std::endl;
}