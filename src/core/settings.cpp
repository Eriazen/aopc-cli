#include "aopc-cli/core/settings.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;


// Singleton instance retrieval, ensuring only one instance of Settings exists throughout the application
Settings& Settings::getInstance() {
    static Settings instance;
    return instance;
}

std::filesystem::path Settings::getDatabasePath() const {
    return databasePath;
}

std::string Settings::getRegionURL() const {
    return regionURL;
}

// Set the database path, ensuring it exists and has the correct extension
bool Settings::setDatabasePath(const std::filesystem::path& path) {
    if (std::filesystem::exists(path) && path.extension() == ".db") {
        databasePath = path.string();
        return true;
    } else {
        return false;
    }
}

// Set the region URL based on a keyword, validating against known regions
bool Settings::setRegionURL(const std::string& keyword) {
    if (keyword == "europe") {
        regionURL = std::string(constants::API_EUROPE_URL);
    } else if (keyword == "americas") {
        regionURL = std::string(constants::API_AMERICAS_URL);
    } else if (keyword == "asia") {
        regionURL = std::string(constants::API_ASIA_URL);
    } else {
        return false;
    }
    return true;
}

bool Settings::loadSettingsFromFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) return false;

    try {
        // Create a JSON object to load file data into
        json j;
        file >> j;

        std::string dbStr = j["database_path"];
        databasePath = std::filesystem::path(dbStr);
        regionURL = j["region_url"];

        return true;
    } catch (json::parse_error& e) {
        // Catch parsing errors (corrupted JSON file, etc.)
        std::cerr << "Error: Couldn't parse file '"<< filePath <<"' with exception '" << e.what() << "'." << std::endl;
        return false;
    }
}

bool Settings::saveSettingsToFile(const std::filesystem::path& filePath) const {
    // Create a JSON object to store currect settings in
    json j;
    j["database_path"] = databasePath.string();
    j["region_url"] = regionURL;

    // Open settings file, overwriting existing settings
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Couldn't create setting file at '" << filePath << "'." << std::endl;
        return false;
    }

    file << j.dump(4);

    return true;
}