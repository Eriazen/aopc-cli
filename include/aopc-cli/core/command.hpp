#pragma once

#include <vector>
#include <string>
#include <memory>


// Base command class
class ICommand {
    public:
        virtual ~ICommand() = default;

        virtual std::unique_ptr<ICommand> create() const = 0;
        virtual void execute(const std::vector<std::string>& args) = 0;
};