#include "aopc-cli/commands/priceCommand.hpp"
#include <iostream>


void PriceCommand::execute(const std::vector<std::string>& args) {
    if (!checkArgs(args, 1)) {
        return;
    }
    std::cout << "Price command executed." << std::endl;
}