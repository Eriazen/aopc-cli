#include "aopc-cli/io/argParser.hpp"
#include <algorithm>
#include <iostream>


// Constructor to initialize the argument parser with the provided arguments
ArgParser::ArgParser(const std::vector<std::string>& arguments) {
    args = arguments;
}

// Check if a specific flag exists in the arguments
bool ArgParser::commandFlagExists(const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

// Get the values associated with a specific flag
std::vector<std::string> ArgParser::getCommandFlagValues(const std::string& flag) {
    std::vector<std::string> values;
    auto it = std::find(args.begin(), args.end(), flag); // Find the flag in the arguments
    if (it != args.end()) {
        ++it; // Move to the next element (the value)
        // Collect values until the next flag or the end of the arguments
        while (it != args.end() && *it != "-" && *it != "--") {
            values.push_back(*it);
            ++it;
        }
    }
    return values;
}

// Check if the number of arguments matches the expected count
bool ArgParser::checkArgs(size_t expectedCount) {
    if (args.size() != expectedCount) {
        std::cout << "Error: Expected " << expectedCount << " arguments, but got " << args.size() << "." << std::endl;
        return false;
    }
    return true;
}