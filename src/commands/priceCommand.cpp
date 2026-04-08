#include "aopc-cli/commands/priceCommand.hpp"
#include "aopc-cli/core/constants.hpp"
#include "aopc-cli/db/itemDatabase.hpp"
#include "aopc-cli/core/settings.hpp"
#include <iostream>
#include <algorithm>


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
}