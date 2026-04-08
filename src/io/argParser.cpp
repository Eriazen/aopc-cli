#include "aopc-cli/io/argParser.hpp"
#include <algorithm>
#include <iostream>


// Constructor: Initialize the argument parser with the provided arguments
ArgParser::ArgParser(const std::vector<std::string>& arguments) {
    args = arguments;
}

// Check if a specific flag exists in the arguments
bool ArgParser::commandFlagExists(const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

// Get the values associated with a specific flag, stopping at the next flag or the end of the arguments
std::vector<std::string> ArgParser::getCommandFlagValues(const std::string& flag) {
    std::vector<std::string> values;
    auto it = std::find(args.begin(), args.end(), flag); // Find the flag in the arguments
    if (it != args.end()) {
        ++it; // Move to the next element (the value)
        // Collect values until the next flag or the end of the arguments
        while (it != args.end() && !it->empty() && it->front() != '-') {
            values.push_back(*it);
            ++it;
        }
    }
    return values;
}

// Check if the number of arguments meets the expected count, and print an error message if not
bool ArgParser::checkMinArgs(size_t minCount) {
    if (args.size() < minCount) {
        std::cout << "Error: Expected at least " << minCount << " arguments, but got " << args.size() << "." << std::endl;
        return false;
    }
    return true;
}

// Check if the number of arguments meets the expected count, and print an error message if not
bool ArgParser::checkExactArgs(size_t expectedCount) {
    if (args.size() != expectedCount) {
        std::cout << "Error: Expected exactly " << expectedCount << " arguments, but got " << args.size() << "." << std::endl;
        return false;
    }
    return true;
}