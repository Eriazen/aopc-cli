#pragma once

#include <vector>
#include <string>


// Base command class
class Command {
    public:
        virtual ~ICommand() = default;

        virtual void execute(const std::vector<std::string>& args) = 0;
};