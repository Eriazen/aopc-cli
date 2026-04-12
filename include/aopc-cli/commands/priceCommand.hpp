#pragma once

#include "aopc-cli/core/command.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/db/itemDatabase.hpp"
#include "aopc-cli/core/settings.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


struct Quality {
    int qualityLevel;
    int marketSellPrice { 0 };
    int taxPaid { 0 };
    int finalProfit { 0 };
};

struct City {
    std::string cityName;
    std::vector<RecipeIngredient> localIngredients;

    int baseMaterialCost { 0 };
    int materialCostWithRrr { 0 };
    int stationFee { 0 };

    std::vector<Quality> qualityProfit;
};

struct PriceReport {
    std::string craftedItemName;
    std::string craftedItemId;
    float appliedRrr { 0.0 };
    float appliedTaxRate { 0.0 };
    int silverCost { 0 };
    int craftingFocus { 0 };
    std::vector<City> cities;
};

class PriceCommand : public ICommand {
    public:
        void execute(const std::vector<std::string>& args) override;

    private:
        PriceReport m_report;
        Settings& m_settings = Settings::getInstance();

        // Argument parsing
        bool getCities(ArgParser& parser);
        bool getQualities(ArgParser& parser);
        // API handling and calculation
        std::string apiURLBuilder();
        void jsonResponseCleanup(json& respondeJson);
        void getMarketPrices(const json& responseJson);
        // Output functions
        void calculateProfit();
        void printPriceReport();
};