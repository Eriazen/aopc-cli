#pragma once

#include "aopc-cli/core/command.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/db/itemDatabase.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class PriceCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override;

    private:
        std::string getItemName(const std::vector<std::string>& args);
        bool getCities(ArgParser& parser, std::vector<std::string>& cities);
        bool getQualities(ArgParser& parser, std::vector<std::string>& qualities);
        std::string apiURLBuilder(const std::string& itemId, const std::vector<RecipeIngredient>& ingredients, const std::vector<std::string>& cities, const std::vector<std::string>& qualities);
        void jsonResponseCleanup(json& respondeJson, const std::vector<RecipeIngredient>& ingredients);
};