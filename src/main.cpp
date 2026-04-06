#include <iostream>
#include "aopc-cli/core/commandHandler.hpp"
#include "aopc-cli/io/natsManager.hpp"


std::string userInput;
CommandHandler handler;

int main(int, char**){
    // Print welcome message
    std::cout << "Albion Online Price Checker CLI tool started!" << std::endl;
    std::cout << "Type 'help' for a list of available commands.\n" << std::endl;

    // Start the command handler loop
    handler.run();

    return 0;
}