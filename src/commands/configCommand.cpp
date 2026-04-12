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

void ConfigCommand::complete(ic_completion_env_t* cenv, const std::string& word, const std::string& line) {
    std::istringstream iss(line);
    std::string token;

    iss >> token; // Discard base command "setconfig"    

    // Count tokens after "help"; stop early if we have 2+
    int token_count = 0;
    while (iss >> token && token_count < 2) {
        ++token_count;
    }

    // No completions if we have 2+ arguments or 1 argument with empty word (already completed)
    if (token_count > 1 || (token_count == 1 && word.empty())) {
        return;
    }

    // Suggest completions for available commands
    for (const auto& [setter_name, _] : m_setters) {
        if (setter_name.find(word) == 0) {
            ic_add_completion(cenv, setter_name.c_str());
        }
    }
}

void ConfigCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);

    if (!parser.checkExactArgs(2)) {
        std::cout << "Error: Invalid config syntax." << '\n';
        std::cout << "Usage: config set <setting_name> <value>" << '\n';
        return;
    }

    std::string settingName = args[0];
    std::string settingValue = args[1];

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