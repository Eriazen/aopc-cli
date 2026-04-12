#pragma once

#include <map>
#include <memory>
#include <functional>
#include "aopc-cli/core/command.hpp"
#include "isocline.h"


// Command handler class
class CommandHandler {
    public:
        CommandHandler();
        void run();

        static void isoclineCompleter(ic_completion_env_t* cenv, const char* input);

    private:
        std::unordered_map<std::string, std::unique_ptr<ICommand>> m_blueprints;
        std::string m_lineContext;

        static void isoclineWordCompleter(ic_completion_env_t* cenv, const char* word);
        void handleCompletion(ic_completion_env_t* cenv, const std::string& word);
};