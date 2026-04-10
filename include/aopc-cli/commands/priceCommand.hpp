#pragma once

#include "aopc-cli/core/command.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/db/itemDatabase.hpp"
#include "aopc-cli/core/settings.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


struct Quality {
    int qualityLevel;
    int marketSellPrice;
    int taxPaid;
    int finalProfit;
};

struct City {
    std::string cityName;
    std::vector<RecipeIngredient> localIngredients;

    int baseMaterialCost;
    int materialCostWithRrr;
    int stationFee;

    std::vector<Quality> qualityProfit;
};

struct PriceReport {
    std::string craftedItemName;
    std::string craftedItemId;
    float appliedRrr;
    float appliedTaxRate;
    int silverCost;
    int craftingFocus;
    std::vector<City> cities;
};

class PriceCommand : public Command {
    public:
        void execute(const std::vector<std::string>& args) override;

    private:
        PriceReport m_report;
        Settings& m_settings = Settings::getInstance();

        bool getCities(ArgParser& parser);
        bool getQualities(ArgParser& parser);
        std::string apiURLBuilder();
        void jsonResponseCleanup(json& respondeJson);
        void getMarketPrices(const json& responseJson);
        void calculateProfit();
        void printPriceReport();
};