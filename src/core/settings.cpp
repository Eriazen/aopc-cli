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

bool Settings::setConfigPath(const std::filesystem::path& path) {
    if (std::filesystem::exists(path) && path.extension() == ".json") {
        m_configPath = path.string();
        return true;
    }

    return false;
}

// Set the database path, ensuring it exists and has the correct extension
bool Settings::setDatabasePath(const std::filesystem::path& path) {
    if (std::filesystem::exists(path) && path.extension() == ".db") {
        m_databasePath = path.string();
        return true;
    }
    
    return false;
}

// Set the region URL based on a keyword, validating against known regions
bool Settings::setRegionURL(const std::string& keyword) {
    if (keyword == "europe") {
        m_regionURL = std::string(constants::API_EUROPE_URL);
    } else if (keyword == "americas") {
        m_regionURL = std::string(constants::API_AMERICAS_URL);
    } else if (keyword == "asia") {
        m_regionURL = std::string(constants::API_ASIA_URL);
    } else {
        return false;
    }

    return true;
}

bool Settings::setMarketTax(const float& taxFraction) {
    if (typeid(taxFraction) == typeid(float)) {
        m_marketTax = taxFraction;
        return true;
    }

    return false;
}

bool Settings::setResourceReturnRate(const float& rrrFraction) {
    if (typeid(rrrFraction) == typeid(float)) {
        m_resourceReturnRate = rrrFraction;
        return true;
    }

    return false;
}

bool Settings::setStationFee(const int& stationFee) {
    if (typeid(m_stationFee) == typeid(int)) {
        m_stationFee = stationFee;
        return true;
    }

    return false;
}

bool Settings::loadSettingsFromFile() {
    std::ifstream file(m_configPath);

    if (!file.is_open()) return false;

    try {
        // Create a JSON object to load file data into
        json j;
        j = json::parse(file);

        std::string dbStr = j["database_path"];
        m_databasePath = std::filesystem::path(dbStr);
        m_regionURL = j["region_url"];
        std::string taxStr = j["market_tax"];
        m_marketTax = std::stof(taxStr);
        std::string rrrStr = j["resource_return_rate"];
        m_resourceReturnRate = std::stof(rrrStr);
        std::string feeStr = j["station_fee"];
        m_stationFee = std::stoi(feeStr);

        return true;
    } catch (const std::exception& e) {
        // Catch parsing errors (corrupted JSON file, etc.)
        std::cerr << "Error: Couldn't parse file '"<< m_configPath <<"' with exception '" << e.what() << "'." << std::endl;
        std::cerr << "Writing default value to a settings file.\n" << std::endl;
        return false;
    }
}

bool Settings::saveSettingsToFile() const {
    // Create a JSON object to store currect settings in
    json j;
    j["database_path"] = m_databasePath.string();
    j["region_url"] = m_regionURL;
    j["market_tax"] = std::to_string(m_marketTax);
    j["resource_return_rate"] = std::to_string(m_resourceReturnRate);
    j["station_fee"] = std::to_string(m_stationFee);

    // Open settings file, overwriting existing settings
    std::ofstream file(m_configPath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Couldn't create setting file at '" << m_configPath << "'." << std::endl;
        return false;
    }

    file << j.dump(4);

    return true;
}