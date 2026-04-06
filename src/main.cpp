#include <iostream>
#include "aopc-cli/io/cmdParser.hpp"

std::string userInput;

int main(int, char**){
    std::cout << "Albion Online Price Checker CLI tool started!" << std::endl;
    std::cout << "Type 'help' for a list of available commands.\n" << std::endl;

    CommandHandler handler;
    handler.run();

    return 0;
}