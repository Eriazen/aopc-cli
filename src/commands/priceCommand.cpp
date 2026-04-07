#include "aopc-cli/commands/priceCommand.hpp"
#include "aopc-cli/core/constants.hpp"
#include "aopc-cli/db/itemDatabase.hpp"
#include <iostream>
#include <algorithm>


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

bool PriceCommand::getCities(ArgParser& parser, std::vector<std::string>& cities) {
    if (parser.commandFlagExists("--cities")) {
        cities = parser.getCommandFlagValues("--cities");
        // Validate the specified cities
        for (const auto& city : cities) {
            if (std::find(constants::CITIES.begin(), constants::CITIES.end(), city) == constants::CITIES.end()) {
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

bool PriceCommand::getQualities(ArgParser& parser, std::vector<std::string>& qualities) {
    if (parser.commandFlagExists("--quality")) {
        qualities = parser.getCommandFlagValues("--quality");
        // Validate the specified qualities
        for (const auto& quality : qualities) {
            if (std::find(constants::ITEM_QUALITIES.begin(), constants::ITEM_QUALITIES.end(), quality) == constants::ITEM_QUALITIES.end()) {
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

// Execute the price command with the provided arguments
void PriceCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);
    std::vector<std::string> cities;
    std::vector<std::string> qualities;

    // Check if the number of arguments is correct
    if (!parser.checkArgs(1)) return;

    // Get the item name from the arguments
    std::string itemName = getItemName(args);
    if (itemName.empty()) {
        std::cout << "Error: No item name provided." << std::endl;
        return;
    }

    // Look up item in the database
    ItemDatabase itemDb(std::string(constants::DB_PATH));
    std::string itemId = itemDb.getItemIdByDisplayName(itemName);
    if (itemId.empty()) {
        std::cout << "Error: Item '" << itemName << "' not found in the database." << std::endl;
        return;
    }

    std::vector<RecipeIngredient> ingredients = itemDb.getRecipeIngredients(itemId);

    // Cities flag handling
    if (!getCities(parser, cities)) return;

    // Quality flag handling
    if (!getQualities(parser, qualities)) return;

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