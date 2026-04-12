#include "aopc-cli/commands/priceCommand.hpp"
#include "aopc-cli/core/constants.hpp"
#include "aopc-cli/io/apiManager.hpp"
#include "aopc-cli/io/formatter.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_set>


// Get the list of cities from the command arguments, validating against known cities
bool PriceCommand::getCities(ArgParser& parser) {
    if (parser.commandFlagExists("--cities")) {
        std::vector<std::string> requestedCities = parser.getCommandFlagValues("--cities");

        // Validate the specified cities, ensuring they are in the list of known cities
        for (const auto& city : requestedCities) {
            if (std::find(constants::CITIES.begin(), constants::CITIES.end(), city) == constants::CITIES.end()) {
                // If an invalid city is found, print an error message and return false
                std::cout << "Error: Invalid city '" << city << "'. Valid cities are: ";
                for (const auto& validCity : constants::CITIES) std::cout << validCity << " ";
                std::cout << std::endl;
                return false;
            }

            City cityData;
            cityData.cityName = city;
            m_report.cities.push_back(cityData);
        }
    } else {
        // If no cities are specified, default to all cities
        for (const auto& city : constants::CITIES) {
            City cityData;
            cityData.cityName = city;
            m_report.cities.push_back(cityData);
        }
    }

    return true;
}

// Get the list of qualities from the command arguments, validating against known qualities
bool PriceCommand::getQualities(ArgParser& parser) {
    std::vector<std::string> validatedQualities;

    if (parser.commandFlagExists("--quality")) {
        std::vector<std::string> requestedQualities = parser.getCommandFlagValues("--quality");

        // Validate the specified qualities, ensuring they are in the list of known qualities
        for (const auto& quality : requestedQualities) {
            if (std::find(constants::ITEM_QUALITIES.begin(), constants::ITEM_QUALITIES.end(), quality) == constants::ITEM_QUALITIES.end()) {
                // If an invalid quality is found, print an error message and return false
                std::cout << "Error: Invalid quality '" << quality << "'. Valid qualities are: ";
                for (const auto& validQuality : constants::ITEM_QUALITIES) std::cout << validQuality << " ";
                std::cout << std::endl;
                return false;
            }

            validatedQualities.push_back(quality);
        }
    } else {
        // If no qualities are specified, default to all qualities
        for (const auto& quality : constants::ITEM_QUALITIES) {
            validatedQualities.push_back(std::string(quality));
        }
    }

    for (auto& city : m_report.cities) {
        for (const auto& quality : validatedQualities) {
            Quality qualityResult;

            qualityResult.qualityLevel = std::stoi(quality);
            city.qualityProfit.push_back(qualityResult);
        }
    }

    return true;
}

// Build an API request url from provided information
std::string PriceCommand::apiURLBuilder() {
    std::string url;

    url.append(m_settings.getRegionURL());
    url.append(std::string(constants::API_PRICE_ENDPOINT));
    url.append(m_report.craftedItemId);

    for (const RecipeIngredient& ingredient : m_report.cities[0].localIngredients) {
        url.append("," + ingredient.materialItemId);
    }

    url.append("?locations=");

    for (const auto& city : m_report.cities) {
        if (city.cityName != m_report.cities.front().cityName) {
            url.append(",");
        }
        url.append(city.cityName);
    }

    url.append("&qualities=");
    
    for (const auto& quality : m_report.cities[0].qualityProfit) {
        if (quality.qualityLevel != m_report.cities[0].qualityProfit.front().qualityLevel) {
            url.append(",");
        }
        url.append(std::to_string(quality.qualityLevel));
    }

    return url;
}

// Response JSON may need clean up from materials with quality higher than 1
void PriceCommand::jsonResponseCleanup(json& responseJson) {
    std::unordered_set<std::string> ingredientIds;
    // Get ingredient IDs to compare against json item IDs later
    for (const auto& ing : m_report.cities[0].localIngredients) {
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

        for (auto& city : m_report.cities) {
            if (city.cityName != itemCity) {
                continue;
            }

            for (auto& quality : city.qualityProfit) {
                if (quality.qualityLevel == itemQuality) {
                    quality.marketSellPrice = marketPrice;
                    break;
                }
            }
            break;
        }
    }

    // Build ingredient ID lookup set for fast filtering
    std::unordered_set<std::string> ingredientIds;
    for (const auto& ing : m_report.cities[0].localIngredients) {
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

        for (auto& city : m_report.cities) {
            if (city.cityName != itemCity) {
                continue;
            }

            for (auto& ingredient : city.localIngredients) {
                if (ingredient.materialItemId == itemId) {
                    ingredient.marketPrice = marketPrice;
                    break;
                }
            }
            break;
        }
    }
}

void PriceCommand::calculateProfit() {
    constexpr float SETUP_FEE = 0.025f;  // 2.5% setup fee

    for (auto& city : m_report.cities) {
        // Calculate total ingredient cost
        int materialCost = 0;
        for (const auto& ingredient : city.localIngredients) {
            materialCost += ingredient.marketPrice * ingredient.quantity;
        }

        city.baseMaterialCost = materialCost;
        
        // Apply resource return rate discount
        float rrr_factor = 1.0f - m_report.appliedRrr;
        city.materialCostWithRrr = static_cast<int>(ceilf(static_cast<float>(materialCost) * rrr_factor));

        // Calculate profit for each quality tier
        for (auto& quality : city.qualityProfit) {
            float tax_base = static_cast<float>(quality.marketSellPrice) * (m_report.appliedTaxRate + SETUP_FEE);
            quality.taxPaid = static_cast<int>(ceilf(tax_base));
            quality.finalProfit = quality.marketSellPrice - quality.taxPaid - city.materialCostWithRrr - m_report.silverCost;
        }
    }
}

void PriceCommand::printPriceReport() {
    std::cout << std::fixed << std::setprecision(1);

    std::cout << std::string(constants::TOTAL_LINE_WIDTH, '=') << '\n';
    std::cout << "REPORT: " << m_report.craftedItemName << '\n';
    std::cout << std::string(constants::TOTAL_LINE_WIDTH, '=') << '\n' << '\n';

    std::cout << "[ Crafting Parameters ]" << '\n';
        std::cout << "  "
            << std::left << std::setw(12) << "Tax Rate" << ": " << std::setw(31) << TextFormatter::formatPercentage(m_report.appliedTaxRate * 100, 1)
            << std::setw(12) << "Return Rate" << ": " << std::setw(31) << TextFormatter::formatPercentage(m_report.appliedRrr * 100, 1) << '\n';

        std::cout << "  "
            << std::left << std::setw(12) << "Silver Cost" << ": " << std::setw(31) << TextFormatter::formatCurrency(m_report.silverCost, "s")
            << std::setw(12) << "Focus Cost" << ": " << TextFormatter::formatNumber(m_report.craftingFocus) << '\n' << '\n';

    for (const auto& city : m_report.cities) {
        unsigned int remainingWidth = constants::TOTAL_LINE_WIDTH - (3 + static_cast<unsigned int>(city.cityName.length()) + 1);
        std::cout << "-- " << city.cityName << " " << std::string(remainingWidth, '-') << '\n' << '\n';

        std::cout << "[ Material Costs ]" << '\n';
        std::cout << "  "
            << std::left << std::setw(30) << "Material Name"
            << std::right << std::setw(10) << "Qty"
            << std::setw(25) << "Market Price"
            << std::setw(25) << "Total Price" << '\n';
        std::cout << "  " << std::string(90, '-') << '\n';

        for (const auto& ingredient : city.localIngredients) {
            std::cout << "  "
                << std::left << std::setw(30) << ingredient.materialItemName
                << std::right << std::setw(10) << TextFormatter::formatNumber(ingredient.quantity)
                << std::setw(25) << TextFormatter::formatCurrency(ingredient.marketPrice, "s")
                << std::setw(25) << TextFormatter::formatCurrency(ingredient.marketPrice * ingredient.quantity, "s") << '\n';
        }
        std::cout << "  " << std::string(90, '-') << '\n';
        std::cout << std::right << std::setw(76) << "Total Base Cost (w/ RRR):" << std::setw(16) << TextFormatter::formatCurrency(city.materialCostWithRrr, "s") << '\n' << '\n';

        std::cout << "[ Projected Profits ]" << '\n';
        std::cout << "  "
            << std::left << std::setw(20) << "Quality"
            << std::right << std::setw(20) << "Market Price"
            << std::setw(20) << "Profit" << '\n';
        std::cout << "  " << std::string(60, '-') << '\n';

        for (const auto& quality : city.qualityProfit) {
            std::cout << "  "
                << std::left << std::setw(20) << TextFormatter::formatNumber(quality.qualityLevel)
                << std::right << std::setw(20) << TextFormatter::formatCurrency(quality.marketSellPrice, "s")
                << std::setw(20) << TextFormatter::formatCurrency(quality.finalProfit, "s") << '\n';
        }
        std::cout << "  " << std::string(60, '-') << '\n' << '\n';

    }

    std::cout << std::defaultfloat;
}

void PriceCommand::complete(ic_completion_env_t* cenv, const std::string& word, const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    std::string activeFlag;
    std::vector<std::string> itemTokens;

    iss >> token; // Discard base command "price" 

    while (iss >> token) {
        if (token.find("--") == 0) {
            activeFlag = token;
            itemTokens.clear();  // Clear item tokens once we hit a flag
        } else if (activeFlag.empty()) {
            itemTokens.push_back(token);
        }
    }

    if (word.find('-') == 0) {
        std::vector<std::string> flags = {"--cities", "--qualities"};
        for (const auto& flag : flags) {
            if (flag.find(word) == 0) ic_add_completion(cenv, flag.c_str());
        }

        return;
    }

    if (activeFlag == "--cities") {
        for (const auto& city : constants::CITIES) {
            if (city.find(word) == 0) ic_add_completion(cenv, std::string(city).c_str());
        }

        return;
    }

    if (activeFlag == "--qualities") {
        for (const auto& q : constants::ITEM_QUALITIES) {
            if (q.find(word) == 0) ic_add_completion(cenv, std::string(q).c_str());
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
        std::cout << "Error: No item name provided." << std::endl;
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
        std::cout << "Error: Item '" << m_report.craftedItemName << "' not found in the database." << std::endl;
        return;
    }

    // Invoke the helper function to get the list of cities from the command arguments, validating against known cities
    if (!getCities(parser)) return;

    // Invoke the helper function to get the list of qualities from the command arguments, validating against known qualities
    if (!getQualities(parser)) return;

    // Fetch the recipe ingredients for the specified item ID from the database
    // Store them in a vector for later use
    std::vector<RecipeIngredient> ingredients = itemDb.getRecipeIngredients(m_report.craftedItemId);
    itemDb.getRecipeIngredientNames(ingredients);
    
    for (auto& city : m_report.cities) {
        city.localIngredients = ingredients;
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

    // Calculate profit margins for each quality tier
    calculateProfit();

    printPriceReport();
}