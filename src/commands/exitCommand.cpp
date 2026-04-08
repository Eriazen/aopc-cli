#include "aopc-cli/commands/exitCommand.hpp"
#include "aopc-cli/io/argParser.hpp"
#include <iostream>


void ExitCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);
    // Check if the number of arguments is correct, and print an error message if not
    if (!parser.checkExactArgs(0)) return;
    
    std::cout << "Exiting..." << std::endl;
    exit(0);
}