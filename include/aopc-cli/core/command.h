#pragma once

#include <vector>
#include <string>


// Base command class
class Command {
    public:
        virtual void execute(const std::vector<std::string>& args) = 0;
        virtual ~Command() = default;
        bool checkArgs(const std::vector<std::string>& args, size_t expected);
};