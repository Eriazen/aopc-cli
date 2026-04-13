#pragma once

#include "aopc-cli/core/command.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/core/settings.hpp"
#include "aopc-cli/core/marketData.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class PriceCommand : public ICommand {
    public:
        std::unique_ptr<ICommand> create() const override { return std::make_unique<PriceCommand>(); };
        void execute(const std::vector<std::string>& args) override;
        void complete(ic_completion_env_t* cenv, const std::string& word, const std::string& line) override;

    private:
        MarketData::PriceReport m_report;
        Settings& m_settings = Settings::getInstance();

        // Argument parsing
        bool getSell(ArgParser& parser);
        bool getBuy(ArgParser& parser);
        void generateRoutes();
        // API handling and calculation
        std::string apiURLBuilder();
        void jsonResponseCleanup(json& respondeJson);
        void getMarketPrices(const json& responseJson);
        // Output functions
        void calculateMaterialCost();
        void calculateSellTax();
        void calculateProfit();
        void printPriceReport();
};