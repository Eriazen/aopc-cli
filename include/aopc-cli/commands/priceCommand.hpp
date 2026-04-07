#pragma once

#include "aopc-cli/core/command.hpp"
#include "aopc-cli/io/argParser.hpp"


class PriceCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override;

    private:
        std::string getItemName(const std::vector<std::string>& args);
        bool getCities(ArgParser& parser, std::vector<std::string>& cities);
        bool getQualities(ArgParser& parser, std::vector<std::string>& qualities);
};