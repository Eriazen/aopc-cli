#pragma once

#include "aopc-cli/core/command.hpp"


class PriceCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override;

    private:
        std::string itemName;
        std::vector<std::string> cities;
};