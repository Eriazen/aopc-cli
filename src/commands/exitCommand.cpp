#include "aopc-cli/commands/exitCommand.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/core/constants.hpp"
#include <iostream>


void ExitCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);
    // Check if the number of arguments is correct, and print an error message if not
    if (!parser.checkExactArgs(0)) return;
    
    std::cout << constants::C_TEXT << "Exiting..." << constants::C_RESET << std::endl;
    exit(0);
}