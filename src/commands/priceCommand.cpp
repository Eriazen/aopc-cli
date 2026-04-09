#include "aopc-cli/commands/priceCommand.hpp"
#include "aopc-cli/core/constants.hpp"
#include "aopc-cli/core/settings.hpp"
#include "aopc-cli/io/apiManager.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_set>


// Extract the item name from the command arguments, stopping at the first flag
std::string PriceCommand::getItemName(const std::vector<std::string>& args) {
    std::string itemName;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i][0] != '-') { // Stop at the first flag
            if (!itemName.empty()) {
                itemName += " ";
            }
            itemName += args[i];
        } else {
            break;
        }
    }
    return itemName;
}

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
            report.cities.push_back(cityData);
        }
    } else {
        // If no cities are specified, default to all cities
        for (const auto& city : constants::CITIES) {
            City cityData;
            cityData.cityName = city;
            report.cities.push_back(cityData);
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

    for (auto& city : report.cities) {
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

    url.append(Settings::getInstance().getRegionURL());
    url.append(std::string(constants::API_PRICE_ENDPOINT));
    url.append(report.craftedItemId);

    for (const RecipeIngredient& ingredient : report.cities[0].localIngredients) {
        url.append("," + ingredient.materialItemId);
    }

    url.append("?locations=");

    for (const auto& city : report.cities) {
        if (city.cityName != report.cities.front().cityName) {
            url.append(",");
        }
        url.append(city.cityName);
    }

    url.append("&qualities=");
    
    for (const auto& quality : report.cities[0].qualityProfit) {
        if (quality.qualityLevel != report.cities[0].qualityProfit.front().qualityLevel) {
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
    for (const auto& ing : report.cities[0].localIngredients) {
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

// Execute the price command, fetching item information and validating cities and qualities
void PriceCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);

    // Check if at least one argument (the item name) is provided, otherwise print an error message and return
    if (!parser.checkMinArgs(1)) return;

    // Extract the item name from the command arguments, stopping at the first flag
    std::string itemName = getItemName(args);
    if (itemName.empty()) {
        std::cout << "Error: No item name provided." << std::endl;
        return;
    }

    // Fetch the item ID from the database using the provided item name
    ItemDatabase itemDb(std::string(Settings::getInstance().getDatabasePath().string()));
    report.craftedItemId = itemDb.getItemIdByDisplayName(itemName);
    // If the item ID is empty, it means the item was not found in the database, so print an error message and return
    if (report.craftedItemId.empty()) {
        std::cout << "Error: Item '" << itemName << "' not found in the database." << std::endl;
        return;
    }

    // Invoke the helper function to get the list of cities from the command arguments, validating against known cities
    if (!getCities(parser)) return;

    // Invoke the helper function to get the list of qualities from the command arguments, validating against known qualities
    if (!getQualities(parser)) return;

    // Fetch the recipe ingredients for the specified item ID from the database
    // Store them in a vector for later use
    std::vector<RecipeIngredient> ingredients = itemDb.getRecipeIngredients(report.craftedItemId);
    for (auto& city : report.cities) {
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

    // For demonstration purposes, print the extracted information to the console
    std::cout << "Price command executed: " << itemName << " (ID: " << report.craftedItemId << ")";
    std::cout << "\nCities: ";
    for (const auto& city : report.cities) {
        std::cout << city.cityName << " ";
    }
    std::cout << "\nQualities: ";
    for (const auto& quality : report.cities[0].qualityProfit) {
        std::cout << quality.qualityLevel << " ";
    }
    std::cout << "\nIngredients:" << std::endl;
    for (const auto& ingredient : report.cities[0].localIngredients) {
        std::cout << " - Ingredient: " << ingredient.materialItemId << ", Quantity: " << ingredient.quantity << std::endl;
    }

    std::cout << prices << std::endl;
}