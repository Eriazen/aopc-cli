#pragma once

#include <vector>
#include <string>
#include <memory>
#include "isocline.h"


// Base command class
class ICommand {
    public:
        virtual ~ICommand() = default;

        virtual std::unique_ptr<ICommand> create() const = 0;
        virtual void execute(const std::vector<std::string>& args) = 0;
        virtual void complete(ic_completion_env_t* cenv, const std::string& word, const std::string& line) {
            (void)cenv;
            (void)word;
            (void)line;
        }
};