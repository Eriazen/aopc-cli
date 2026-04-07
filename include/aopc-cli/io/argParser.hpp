#pragma once

#include <string>
#include <vector>


// Class to parse command-line arguments
class ArgParser {
    public:
        ArgParser(const std::vector<std::string>& arguments);
        ~ArgParser() = default;

        // Check if a specific flag exists in the arguments
        bool commandFlagExists(const std::string& flag);
        // Get the values associated with a specific flag
        std::vector<std::string> getCommandFlagValues(const std::string& flag);

        bool checkArgs(size_t expectedCount);

    private:
        std::vector<std::string> args;
};