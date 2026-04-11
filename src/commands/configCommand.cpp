#include "aopc-cli/commands/configCommand.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/core/settings.hpp"
#include <iostream>


ConfigCommand::ConfigCommand() {
    m_setters["database_path"] = [](const std::string& val) {
        if (Settings::getInstance().setDatabasePath(val)) {
            std::cout << "Database path set to '" << val << "'." << '\n';
        } else {
            std::cout << "Error: Invalid database path '" << val << "'." << '\n';
        }
    };

    m_setters["region"] = [](const std::string& val) {
        if (Settings::getInstance().setRegionURL(val)) {
            std::cout << "Region set to '" << val << "'." << '\n';
        } else {
            std::cout << "Invalid region '" << val << "'." << '\n';
        }
    };
    
    m_setters["market_tax"] = [](const std::string& val) {
        float num = std::stof(val);
        if (Settings::getInstance().setMarketTax(num)) {
            std::cout << "Market tax set to " << val << "." << '\n';
        } else {
            std::cout << "Invalid market tax value '" << num << "'." << '\n';
        }
    };

    m_setters["resource_return"] = [](const std::string& val) {
        float num = std::stof(val);
        if (Settings::getInstance().setResourceReturnRate(num)) {
            std::cout << "Resource return rate set to " << val << "." << '\n';
        } else {
            std::cout << "Invalid resource return rate value '" << num << "'." << '\n';
        }
    };

    m_setters["station_fee"] = [](const std::string& val) {
        int num = std::stoi(val);
        if (Settings::getInstance().setStationFee(num)) {
            std::cout << "Station fee set to " << val << "." << '\n';
        } else {
            std::cout << "Invalid station fee value '" << num << "'." << '\n';
        }
    };
}

void ConfigCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);

    if (!parser.checkMinArgs(3) || args[0] != "set") {
        std::cout << "Error: Invalid config syntax." << '\n';
        std::cout << "Usage: config set <setting_name> <value>" << '\n';
        return;
    }

    std::string settingName = args[1];
    std::string settingValue = args[2];

    auto it = m_setters.find(settingName);
    if (it != m_setters.end()) {
        try {
            it->second(settingValue);

            Settings::getInstance().saveSettingsToFile();
        } catch (const std::invalid_argument&) {
            std::cout << "Error: Invalid value format for '" << settingName << "'. Expected a number." << '\n'; 
        } catch (const std::out_of_range&) {
            std::cout << "Error: The number provided is too large or too small." << '\n';
        }
    } else {
        std::cout << "Error: Unknown setting '" << settingName << "'." << '\n';
        std::cout << "Type 'help config' for a list of valid settings." << '\n';
    }
}