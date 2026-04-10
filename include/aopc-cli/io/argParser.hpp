#pragma once

#include <string>
#include <vector>


// Class to parse command-line arguments
class ArgParser {
    public:
        ArgParser(const std::vector<std::string>& arguments) : m_args(arguments) {};

        // Check if a specific flag exists in the arguments
        bool commandFlagExists(const std::string& flag);
        // Get the values associated with a specific flag
        std::vector<std::string> getCommandFlagValues(const std::string& flag);

        bool checkExactArgs(size_t expectedCount);
        bool checkMinArgs(size_t minCount);

    private:
        std::vector<std::string> m_args;
};