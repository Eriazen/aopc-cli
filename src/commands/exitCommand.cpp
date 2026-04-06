#include "aopc-cli/commands/exitCommand.hpp"
#include <iostream>

void ExitCommand::execute(const std::vector<std::string>& args) {
    if (!checkArgs(args, 0)) {
        return;
    }
    std::cout << "Exiting..." << std::endl;
    exit(0);
}