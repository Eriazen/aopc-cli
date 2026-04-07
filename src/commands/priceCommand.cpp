#include "aopc-cli/commands/priceCommand.hpp"
#include "aopc-cli/io/argParser.hpp"
#include <iostream>


// Execute the price command with the provided arguments
void PriceCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);
    // Check if the number of arguments is correct
    if (!parser.checkArgs(1)) {
        return;
    }
    std::cout << "Price command executed." << std::endl;
}