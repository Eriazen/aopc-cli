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
bool PriceCommand::getCities(ArgParser& parser, std::vector<std::string>& cities) {
    if (parser.commandFlagExists("--cities")) {
        cities = parser.getCommandFlagValues("--cities");
        // Validate the specified cities, ensuring they are in the list of known cities
        for (const auto& city : cities) {
            if (std::find(constants::CITIES.begin(), constants::CITIES.end(), city) == constants::CITIES.end()) {
                // If an invalid city is found, print an error message and return false
                std::cout << "Error: Invalid city '" << city << "'. Valid cities are: ";
                for (const auto& validCity : constants::CITIES) std::cout << validCity << " ";
                std::cout << std::endl;
                return false;
            }
        }
    } else {
        // If no cities are specified, default to all cities
        for (const auto& city : constants::CITIES) {
            cities.push_back(std::string(city));
        }
    }
    return true;
}

// Get the list of qualities from the command arguments, validating against known qualities
bool PriceCommand::getQualities(ArgParser& parser, std::vector<std::string>& qualities) {
    if (parser.commandFlagExists("--quality")) {
        qualities = parser.getCommandFlagValues("--quality");
        // Validate the specified qualities, ensuring they are in the list of known qualities
        for (const auto& quality : qualities) {
            if (std::find(constants::ITEM_QUALITIES.begin(), constants::ITEM_QUALITIES.end(), quality) == constants::ITEM_QUALITIES.end()) {
                // If an invalid quality is found, print an error message and return false
                std::cout << "Error: Invalid quality '" << quality << "'. Valid qualities are: ";
                for (const auto& validQuality : constants::ITEM_QUALITIES) std::cout << validQuality << " ";
                std::cout << std::endl;
                return false;
            }
        }
    } else {
        // If no qualities are specified, default to all qualities
        for (const auto& quality : constants::ITEM_QUALITIES) {
            qualities.push_back(std::string(quality));
        }
    }
    return true;
}

// Build an API request url from provided information
std::string PriceCommand::apiURLBuilder(const std::string& itemId, const std::vector<RecipeIngredient>& ingredients, const std::vector<std::string>& cities, const std::vector<std::string>& qualities) {
    std::string url;

    url.append(Settings::getInstance().getRegionURL());
    url.append(std::string(constants::API_PRICE_ENDPOINT));
    url.append(itemId);

    for (const RecipeIngredient& ingredient : ingredients) {
        url.append("," + ingredient.materialItemId);
    }

    url.append("?locations=");

    for (const std::string& city : cities) {
        if (city != cities.front()) {
            url.append(",");
        }
        url.append(city);
    }

    url.append("&qualities=");
    
    for (const std::string& quality : qualities) {
        if (quality != qualities.front()) {
            url.append(",");
        }
        url.append(quality);
    }

    return url;
}

// Response JSON may need clean up from materials with quality higher than 1
void PriceCommand::jsonResponseCleanup(json& responseJson, const std::vector<RecipeIngredient>& ingredients) {
    std::unordered_set<std::string> ingredientIds;
    // Get ingredient IDs to compare against json item IDs later
    for (const auto& ing : ingredients) {
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
    std::vector<std::string> cities;
    std::vector<std::string> qualities;

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
    std::string itemId = itemDb.getItemIdByDisplayName(itemName);
    // If the item ID is empty, it means the item was not found in the database, so print an error message and return
    if (itemId.empty()) {
        std::cout << "Error: Item '" << itemName << "' not found in the database." << std::endl;
        return;
    }

    // Fetch the recipe ingredients for the specified item ID from the database
    // Store them in a vector for later use
    std::vector<RecipeIngredient> ingredients = itemDb.getRecipeIngredients(itemId);

    // Invoke the helper function to get the list of cities from the command arguments, validating against known cities
    if (!getCities(parser, cities)) return;

    // Invoke the helper function to get the list of qualities from the command arguments, validating against known qualities
    if (!getQualities(parser, qualities)) return;

    // Build API url from known data, appending ingredients, cities and qualities
    std::string url = apiURLBuilder(itemId, ingredients, cities, qualities);

    // Initialize APIManager, make a request and parse recieved JSON response
    APIManager api(url);
    api.performCurlRequest();
    api.parseJsonResponse();
    json prices = api.getJsonResponse();
    jsonResponseCleanup(prices, ingredients);



    // For demonstration purposes, print the extracted information to the console
    std::cout << "Price command executed: " << itemName << " (ID: " << itemId << ")";
    std::cout << "\nCities: ";
    for (const auto& city : cities) {
        std::cout << city << " ";
    }
    std::cout << "\nQualities: ";
    for (const auto& quality : qualities) {
        std::cout << quality << " ";
    }
    std::cout << "\nIngredients:" << std::endl;
    for (const auto& ingredient : ingredients) {
        std::cout << " - Ingredient: " << ingredient.materialItemId << ", Quantity: " << ingredient.quantity << std::endl;
    }

    std::cout << prices << std::endl;
}