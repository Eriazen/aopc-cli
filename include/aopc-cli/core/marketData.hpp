#pragma once

#include <vector>
#include <string>
#include "aopc-cli/db/itemDatabase.hpp"


namespace MarketData
{
    struct MarketSellData {
        std::string cityName;
        int qualityLevel;
        int marketSellPrice { 0 };
        int taxPaid { 0 };
    };

    struct MaterialSourceData {
        std::string cityName;
        std::vector<RecipeIngredient> localIngredients;

        int baseMaterialCost { 0 };
        int materialCostWithRrr { 0 };
        int stationFee { 0 };

        int getTotalCost() const { return materialCostWithRrr + stationFee; }
    };

    struct TradeRoute {
        std::string sourceCity;
        std::string destinationCity;
        int qualityLevel;

        int totalCost { 0 };
        int totalRevenue { 0 };
        int finalProfit { 0 };
    };

    struct PriceReport {
        std::string craftedItemName;
        std::string craftedItemId;
        float appliedRrr { 0.0f };
        float appliedTaxRate { 0.0f };
        int silverCost { 0 };
        int craftingFocus { 0 };

        std::vector<MaterialSourceData> materialSources;
        std::vector<MarketSellData> sellMarkets;
        std::vector<TradeRoute> calculatedRoutes;
    };
} // namespace MarketData

