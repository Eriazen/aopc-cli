#include "aopc-cli/commands/priceCommand.hpp"
#include "aopc-cli/core/constants.hpp"
#include "aopc-cli/io/apiManager.hpp"
#include "aopc-cli/io/formatter.hpp"
#include "aopc-cli/db/itemDatabase.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>


// Get the list of cities from the command arguments, validating against known cities
bool PriceCommand::getSell(ArgParser& parser) {
    std::unordered_set<std::string> sellCities;

    if (parser.commandFlagExists("--sell")) {
        std::vector<std::string> requestedCities = parser.getCommandFlagValues("--sell");

        // Validate the specified cities, ensuring they are in the list of known cities
        for (const auto& city : requestedCities) {
            if (std::find(constants::CITIES.begin(), constants::CITIES.end(), city) == constants::CITIES.end()) {
                // If an invalid city is found, print an error message and return false
                std::cout << "Error: Invalid city '" << city << "'. Valid cities are: ";
                for (const auto& validCity : constants::CITIES) std::cout << validCity << " ";
                std::cout << std::endl;
                return false;
            }

            if (sellCities.count(city) == 0) {
                sellCities.insert(city);
            }
        }
    } else {
        // If no cities are specified, default to all cities
        for (const auto& city : constants::CITIES) {
            sellCities.insert(std::string(city));
        }
    }

    for (const auto& city : sellCities) {
        for (const auto& quality : constants::ITEM_QUALITIES) {
            MarketData::MarketSellData citySellData;
            citySellData.cityName = city;
            citySellData.qualityLevel = quality;
            m_report.sellMarkets.push_back(citySellData);
        }
    }


    return true;
}

// Get the list of qualities from the command arguments, validating against known qualities
bool PriceCommand::getBuy(ArgParser& parser) {
    std::unordered_set<std::string> buyCities;

    if (parser.commandFlagExists("--buy")) {
        std::vector<std::string> requestedCities = parser.getCommandFlagValues("--buy");

        // Validate the specified cities, ensuring they are in the list of known cities
        for (const auto& city : requestedCities) {
            if (std::find(constants::CITIES.begin(), constants::CITIES.end(), city) == constants::CITIES.end()) {
                // If an invalid city is found, print an error message and return false
                std::cout << constants::C_ERROR << "Error: Invalid city '" << city << "'. " << constants::C_TEXT << "Valid cities are: ";
                for (const auto& validCity : constants::CITIES) std::cout << validCity << " ";
                std::cout << std::endl;
                return false;
            }

            if (buyCities.count(city) == 0) {
                buyCities.insert(city);
            }
        }
    }
    
    for (const auto& city : buyCities) {
        MarketData::MaterialSourceData cityBuyData;
        cityBuyData.cityName = city;
        m_report.materialSources.push_back(cityBuyData);
    }

    return true;
}

// Generate trade routes from available sources/destinations
void PriceCommand::generateRoutes() {
    m_report.calculatedRoutes.clear();

    if (m_report.materialSources.empty()) {
        for (const auto& dest : m_report.sellMarkets) {
            MarketData::TradeRoute route;
            route.destinationCity = dest.cityName;
            route.qualityLevel = dest.qualityLevel;

            m_report.calculatedRoutes.push_back(route);
        }
    } else {
        for(const auto& source : m_report.materialSources) {
            for (const auto& dest : m_report.sellMarkets) {
                MarketData::TradeRoute route;
                route.destinationCity = dest.cityName;
                route.sourceCity = source.cityName;
                route.qualityLevel = dest.qualityLevel;

                m_report.calculatedRoutes.push_back(route);
            }
        }
    }
}

// Build an API request url from provided information
std::string PriceCommand::apiURLBuilder() {
    std::string url;

    url.append(m_settings.getRegionURL());
    url.append(std::string(constants::API_PRICE_ENDPOINT));
    url.append(m_report.craftedItemId);

    if (!m_report.materialSources.empty()) {
        for (const RecipeIngredient& ingredient : m_report.materialSources[0].localIngredients) {
            url.append("," + ingredient.materialItemId);
        }
    }

    url.append("?locations=");

    std::unordered_set<std::string> uniqueSellCities;
    for (const auto& market : m_report.sellMarkets) {
        if (uniqueSellCities.insert(market.cityName).second) {
            if (uniqueSellCities.size() > 1) {
                url.append(",");
            }
            url.append(market.cityName);
        }
    }

    for (const auto& city : m_report.materialSources) {
        if (url.find(city.cityName) != std::string::npos) {
            continue;
        }
        url.append(",");
        url.append(city.cityName);
    }

    return url;
}

// Response JSON may need clean up from materials with quality higher than 1
void PriceCommand::jsonResponseCleanup(json& responseJson) {
    if (m_report.materialSources.empty()) return;

    std::unordered_set<std::string> ingredientIds;
    // Get ingredient IDs to compare against json item IDs later
    for (const auto& ing : m_report.materialSources[0].localIngredients) {
        ingredientIds.insert(ing.materialItemId);
    }

    for (auto it = responseJson.begin(); it != responseJson.end();) {
        std::string currentItemId = (*it)["item_id"];
        int quality = (*it)["quality"];

        // Erase all materials with quality higher than 1
        if (ingredientIds.count(currentItemId) && quality != 1) {
            it = responseJson.erase(it);
        } else {
            ++it;
        }
    }
}

void PriceCommand::getMarketPrices(const json& responseJson) {
    for (const auto& item : responseJson) {
        if (item["item_id"] != m_report.craftedItemId) {
            continue;
        }

        std::string itemCity = item["city"];
        int itemQuality = item["quality"];
        int marketPrice = item["sell_price_min"];

        itemCity.erase(std::remove_if(itemCity.begin(), itemCity.end(), ::isspace), itemCity.end());

        for (auto& cityMarket : m_report.sellMarkets) {
            if (cityMarket.cityName == itemCity && cityMarket.qualityLevel == itemQuality) {
                cityMarket.marketSellPrice = marketPrice;
                break;
            }
        }
    }

    // Don't process ingredient prices if there aren't any
    if (m_report.materialSources.empty()) return;

    // Build ingredient ID lookup set for fast filtering
    std::unordered_set<std::string> ingredientIds;
    for (const auto& ing : m_report.materialSources[0].localIngredients) {
        ingredientIds.insert(ing.materialItemId);
    }
    
    // Process ingredient prices
    for (const auto& item : responseJson) {
        std::string itemId = item["item_id"];
        
        // Skip if it's the crafted item or not an ingredient
        if (itemId == m_report.craftedItemId || !ingredientIds.count(itemId)) {
            continue;
        }

        std::string itemCity = item["city"];
        int marketPrice = item["sell_price_min"];

        itemCity.erase(std::remove_if(itemCity.begin(), itemCity.end(), ::isspace), itemCity.end());

        for (auto& source : m_report.materialSources) {
            if (source.cityName != itemCity) {
                continue;
            }

            for (auto& ingredient : source.localIngredients) {
                if (ingredient.materialItemId == itemId) {
                    ingredient.marketPrice = marketPrice;
                    break;
                }
            }
            break;
        }
    }
}

void PriceCommand::calculateMaterialCost() {
    for (auto& source : m_report.materialSources) {
        // Calculate total ingredient cost
        int materialCost { 0 };
        for (const auto& ingredient : source.localIngredients) {
            materialCost += ingredient.marketPrice * ingredient.quantity;
        }

        source.baseMaterialCost = materialCost;

        // Apply resource return rate discount
        float rrrFactor = 1.0f - m_report.appliedRrr;
        source.materialCostWithRrr = static_cast<int>(ceilf(static_cast<float>(materialCost) * rrrFactor));
    }
}

void PriceCommand::calculateProfit() {
    constexpr float SETUP_FEE = 0.025f;  // 2.5% setup fee

    for (auto& market : m_report.sellMarkets) {
        float taxBase = static_cast<float>(market.marketSellPrice) * (m_report.appliedTaxRate + SETUP_FEE);
        market.taxPaid = static_cast<int>(ceilf(taxBase));
    }

    for (auto& route : m_report.calculatedRoutes) {
        for (const auto& market : m_report.sellMarkets) {

            if (route.destinationCity == market.cityName && route.qualityLevel == market.qualityLevel) {
                route.totalRevenue = market.marketSellPrice - market.taxPaid;
                break;
            }
        }

        for (const auto& source : m_report.materialSources) {
            if (route.sourceCity == source.cityName) {
                route.totalCost = source.materialCostWithRrr;
                break;
            }
        }

        route.finalProfit = route.totalRevenue - route.totalCost - m_report.silverCost;
    }
}

void PriceCommand::printPriceReport() {
    std::cout << std::fixed << std::setprecision(1);

    std::cout << constants::C_TEXT << std::string(constants::TOTAL_LINE_WIDTH, '=') << '\n';
    std::cout << constants::C_HL2 << "REPORT: " << constants::C_RESET << m_report.craftedItemName << '\n';
    std::cout << constants::C_TEXT << std::string(constants::TOTAL_LINE_WIDTH, '=') << "\n\n";

    std::cout << "[ Global Parameters ]\n";
        std::cout << constants::C_RESET << "  "
            << std::left << std::setw(12) << "Tax Rate" << ": " << std::setw(31) << TextFormatter::formatPercentage(m_report.appliedTaxRate * 100, 1)
            << std::setw(12) << "Return Rate" << ": " << std::setw(31) << TextFormatter::formatPercentage(m_report.appliedRrr * 100, 1) << '\n';
        if (!m_report.materialSources.empty()) {
            std::cout << "  "
                << std::left << std::setw(12) << "Silver Cost" << ": " << std::setw(31) << TextFormatter::formatCurrency(m_report.silverCost, "s")
                << std::setw(12) << "Focus Cost" << ": " << TextFormatter::formatNumber(m_report.craftingFocus) << '\n';
        }
    
    std::cout << '\n';

    std::vector<std::string> uniqueDestCities;
    for (const auto& market : m_report.sellMarkets) {
        if (std::find(uniqueDestCities.begin(), uniqueDestCities.end(), market.cityName) == uniqueDestCities.end()) {
            uniqueDestCities.push_back(market.cityName);
        }
    }

    if (!m_report.materialSources.empty()) {
        
        // Crafting mode: source -> destination
        for (const auto& source : m_report.materialSources) {
            for (const auto& destCityName : uniqueDestCities) {
                
                std::string cityHeader { source.cityName + " --> " + destCityName };
                
                // Safe width calculation
                unsigned int remainingWidth = 0;
                if (constants::TOTAL_LINE_WIDTH > (3 + cityHeader.length() + 1)) {
                    remainingWidth = constants::TOTAL_LINE_WIDTH - (3 + static_cast<unsigned int>(cityHeader.length()) + 1);
                }
                std::cout << constants::C_TEXT << "-- " << constants::C_HL2 << cityHeader << constants::C_TEXT << " " << std::string(remainingWidth, '-') << "\n\n";

                // Material Table
                std::cout << "[ Material Costs ]\n";
                std::cout << constants::C_HL3 << "  "
                    << std::left << std::setw(30) << "Material Name"
                    << std::right << std::setw(10) << "Qty"
                    << std::setw(25) << "Market Price"
                    << std::setw(25) << "Total Price\n";
                std::cout << constants::C_TEXT << "  " << std::string(90, '-') << constants::C_RESET << '\n';

                for (const auto& ingredient : source.localIngredients) {
                    std::cout << "  "
                        << std::left << std::setw(30) << ingredient.materialItemName
                        << std::right << std::setw(10) << TextFormatter::formatNumber(ingredient.quantity)
                        << std::setw(25) << TextFormatter::formatCurrency(ingredient.marketPrice, "s")
                        << std::setw(25) << TextFormatter::formatCurrency(ingredient.marketPrice * ingredient.quantity, "s") << '\n';
                }
                std::cout << constants::C_TEXT << "  " << std::string(90, '-') << constants::C_RESET << '\n';

                std::cout << std::right << std::setw(76) << "Total Base Cost (w/ RRR):" << std::setw(16)
                    << TextFormatter::formatCurrency(source.materialCostWithRrr, "s") << "\n\n";

                // Profit Table
                std::cout << constants::C_TEXT << "[ Projected Profits ]\n";
                std::cout << constants::C_HL3 << "  "
                    << std::left << std::setw(20) << "Quality"
                    << std::right << std::setw(20) << "Net Revenue"
                    << std::setw(20) << "Profit" << '\n';
                std::cout << constants::C_TEXT << "  " << std::string(60, '-') << constants::C_RESET << '\n';

                // Loop through calculated routes to find the exact permutations for this Source/Dest combo
                for (const auto& route : m_report.calculatedRoutes) {
                    if (route.sourceCity == source.cityName && route.destinationCity == destCityName) {
                        std::string profitColor;
                        if (route.finalProfit > 0) {
                            profitColor = constants::C_SUCCESS;
                        } else {
                            profitColor = constants::C_ERROR;
                        }
                        std::cout << "  "
                            << std::left << std::setw(20) << TextFormatter::formatNumber(route.qualityLevel)
                            << std::right << std::setw(20) << TextFormatter::formatCurrency(route.totalRevenue, "s")
                            << profitColor << std::setw(20) << TextFormatter::formatCurrency(route.finalProfit, "s") << constants::C_RESET << '\n';
                    }
                }
                std::cout << constants::C_TEXT << "  " << std::string(60, '-') << constants::C_RESET << "\n\n";
            }
        }
    } else {
        
        // Sell mode
        for (const auto& destCityName : uniqueDestCities) {
            
            // Safe width calculation
            unsigned int remainingWidth = 0;
            if (constants::TOTAL_LINE_WIDTH > (3 + destCityName.length() + 1)) {
                remainingWidth = constants::TOTAL_LINE_WIDTH - (3 + static_cast<unsigned int>(destCityName.length()) + 1);
            }
            std::cout << constants::C_TEXT << "-- " << constants::C_HL2 << destCityName << constants::C_TEXT << " " << std::string(remainingWidth, '-') << "\n\n";

            // Profit Table
            std::cout << "[ Projected Profits ]\n";
            std::cout << constants::C_HL3 << "  "
                << std::left << std::setw(20) << "Quality"
                << std::right << std::setw(20) << "           "
                << std::setw(20) << "Profit" << '\n';
            std::cout << constants::C_TEXT << "  " << std::string(60, '-') << constants::C_RESET << '\n';

            // Filter routes for this specific destination
            for (const auto& route : m_report.calculatedRoutes) {
                if (route.destinationCity == destCityName) {
                    std::string profitColor;
                    if (route.finalProfit > 0) {
                        profitColor = constants::C_SUCCESS;
                    } else {
                        profitColor = constants::C_ERROR;
                    }
                    std::cout << "  "
                        << std::left << std::setw(20) << TextFormatter::formatNumber(route.qualityLevel)
                        << std::right << std::setw(20) << std::string(TextFormatter::formatCurrency(route.totalRevenue, "s").length(), ' ')
                        << profitColor << std::setw(20) << TextFormatter::formatCurrency(route.finalProfit, "s") << constants::C_RESET << '\n';
                }
            }
            std::cout << constants::C_TEXT << "  " << std::string(60, '-') << constants::C_RESET << "\n\n";
        }
    }

    std::cout << std::defaultfloat;
}

void PriceCommand::complete(ic_completion_env_t* cenv, const std::string& word, const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    std::string activeFlag;
    std::vector<std::string> itemTokens;
    std::vector<std::string> activeFlagArgs;

    iss >> token; // Discard base command "price" 

    while (iss >> token) {
        if (token.find("--") == 0) {
            activeFlag = token;
            itemTokens.clear();  // Clear item tokens once we hit a flag
            activeFlagArgs.clear();
        } else if (activeFlag.empty()) {
            itemTokens.push_back(token);
        } else {
            activeFlagArgs.push_back(token);
        }
    }

    if (word.find('-') == 0) {
        std::vector<std::string> flags = {"--buy", "--sell"};
        for (const auto& flag : flags) {
            if (flag.find(word) == 0 && line.find(flag) == std::string::npos) {
                ic_add_completion(cenv, flag.c_str());
            }
        }

        return;
    }

    if (activeFlag == "--buy" || activeFlag == "--sell") {
        for (const auto& city : constants::CITIES) {
            if (city.find(word) == 0) {
                if (std::find(activeFlagArgs.begin(), activeFlagArgs.end(), city) == activeFlagArgs.end()) {
                    ic_add_completion(cenv, std::string(city).c_str());
                }
            }
        }

        return;
    }

    if (activeFlag.empty()) {
        // Don't try to complete empty words (user just typed a space)
        if (word.empty()) {
            return;
        }

        ItemDatabase itemDb(m_settings.getDatabasePath().string());
        
        std::string searchTerm;
        
        // Build search term: all previous complete tokens plus the current partial word
        for (size_t i = 0; i < itemTokens.size(); ++i) {
            if (!searchTerm.empty()) searchTerm += " ";
            searchTerm += itemTokens[i];
        }
        
        std::vector<std::string> nameMatches = itemDb.getItemNameMatches(searchTerm);
        for (const auto& match : nameMatches) {
            // Only suggest if the match is longer than what's already typed
            if (match.find(searchTerm) == 0 && match.length() > searchTerm.length()) {
                // Validate that the item actually exists in the database with a valid ID
                ItemInfo itemInfo = itemDb.getItemInfoByDisplayName(match);
                if (!itemInfo.itemId.empty()) {
                    // Calculate how much of the search term belongs to previously completed words.
                    size_t previousTokensLength = searchTerm.length() - word.length();
                    
                    // Extract the remainder of the match starting from the current word
                    std::string completionStr = match.substr(previousTokensLength);
                    
                    ic_add_completion(cenv, completionStr.c_str());
                }
            }
        }
    }
}

// Execute the price command, fetching item information and validating cities and qualities
void PriceCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);

    // Check if at least one argument (the item name) is provided, otherwise print an error message and return
    if (!parser.checkMinArgs(1)) return;

    // Extract the item name from the command arguments, stopping at the first flag
    std::vector<std::string> itemNameVector = parser.getPreFlagValues();
    for (const auto& word : itemNameVector) {
        if (word != itemNameVector[0]) {
            m_report.craftedItemName += " ";
        }

        m_report.craftedItemName += word;
    }

    if (m_report.craftedItemName.empty()) {
        std::cout << constants::C_ERROR << "Error: No item name provided." << constants::C_RESET<< '\n';
        return;
    }

    // Fetch the item ID from the database using the provided item name
    ItemDatabase itemDb(std::string(m_settings.getDatabasePath().string()));
    ItemInfo info = itemDb.getItemInfoByDisplayName(m_report.craftedItemName);
    m_report.craftedItemId = info.itemId;
    m_report.silverCost = info.silverCost;
    m_report.craftingFocus = info.craftingFocus;

    // If the item ID is empty, it means the item was not found in the database, so print an error message and return
    if (m_report.craftedItemId.empty()) {
        std::cout << constants::C_ERROR << "Error: Item '" << m_report.craftedItemName << "' not found in the database." << constants::C_RESET<< '\n';
        return;
    }

    // Invoke the helper function to get the list of cities from the command arguments, validating against known cities
    if (!getSell(parser)) return;

    // Invoke the helper function to get the list of qualities from the command arguments, validating against known qualities
    if (!getBuy(parser)) return;

    // Generate trade routes based on the sell and buy flags
    generateRoutes();

    // Fetch the recipe ingredients for the specified item ID from the database
    // Store them in a vector for later use
    if (!m_report.materialSources.empty()) {
        std::vector<RecipeIngredient> ingredients = itemDb.getRecipeIngredients(m_report.craftedItemId);
        itemDb.getRecipeIngredientNames(ingredients);
        
        for (auto& source : m_report.materialSources) {
            source.localIngredients = ingredients;
        }
    }

    // Build API url from known data, appending ingredients, cities and qualities
    std::string url = apiURLBuilder();

    // Initialize APIManager, make a request and parse recieved JSON response
    APIManager api(url);
    api.performCurlRequest();
    api.parseJsonResponse();
    json prices = api.getJsonResponse();
    jsonResponseCleanup(prices);

    // Extract market prices from response JSON, get necessary values from Settings
    getMarketPrices(prices);
    m_report.appliedTaxRate = m_settings.getMarketTax();
    m_report.appliedRrr = m_settings.getResourceReturnRate();

    if (!m_report.materialSources.empty()) {
        calculateMaterialCost();
    }
    // Calculate profit margins for each quality tier
    calculateProfit();

    printPriceReport();
}