#include "aopc-cli/core/command.h"
#include <iostream>


// Base command implementation
bool Command::checkArgs(const std::vector<std::string>& args, size_t expected) {
    if (args.size() != expected) {
        std::cout << "Invalid number of arguments. Expected " << expected << " argument(s)." << std::endl;
        return false;
    }
    return true;
}