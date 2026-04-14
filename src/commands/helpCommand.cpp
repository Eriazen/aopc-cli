#include "aopc-cli/commands/helpCommand.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/core/constants.hpp"
#include "aopc-cli/io/formatter.hpp"
#include <iostream>
#include <sstream>


HelpCommand::HelpCommand() {
    m_commands["price"] = [this]() { return printPriceHelp(); };
    m_commands["setconfig"] = [this]() { return printConfigHelp(); };
}

void HelpCommand::complete(ic_completion_env_t* cenv, const std::string& word, const std::string& line) {
    std::istringstream iss(line);
    std::string token;

    iss >> token; // Discard base command "help"

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
    for (const auto& [cmd_name, _] : m_commands) {
        if (cmd_name.find(word) == 0) {
            ic_add_completion(cenv, cmd_name.c_str());
        }
    }
}

void HelpCommand::execute(const std::vector<std::string>& args) {
    ArgParser parser(args);
    // Check if the number of arguments is correct, and print an error message if not
    if (parser.checkExactArgs(0, false)) {
        printGeneralHelp();
        return;
    }

    std::string target = args[0];

    auto it = m_commands.find(target);
    if (it != m_commands.end()) {
        it->second();
    } else {
        std::cout << "No detailed help available for '" << target << "'." << '\n';
        std::cout << "Type 'help' for a list of available commands." << '\n';
    }
}

void HelpCommand::printGeneralHelp() const {
    std::cout << R"(
====================================================================================================
Albion Online Profit Calculator - Help Menu
====================================================================================================

USAGE:
    > <command> [arguments] [flags]

COMMANDS:
    price       Check the market price and calculate profit for crafting an item.
    setconfig   Manage application settings and preferences.
    help        Show this help message.
    exit        Close the application.

Type 'help <command>' for detailed information.
)" << '\n';
}

void HelpCommand::printPriceHelp() const {
    std::cout << R"(
--- Help: price ---
Calculates crafting profitability and cross-references market prices across multiple cities.

USAGE:
    > price <item_name> [--sell <city1> <city2>...] [--buy <city1> <city2>...]

OPTIONS:
    --buy <cities>  Calculate material crafting costs sourced from these cities. 
                    If omitted, the command skips materials and runs in "Sell-Only" mode.
                    [Default: none]

    --sell <cities> Check final item market prices and profit margins in these target cities.
                    [Default: all cities]

                  Valid cities: )";

    std::string lineIndent = "                                ";
    TextFormatter::printWrappedList(constants::CITIES, lineIndent, constants::TOTAL_LINE_WIDTH);

    std::cout << R"(

EXAMPLES:
    # Check sell prices for planks across all cities (Sell-Only mode)
    > price Exceptional Ashenbark Planks

    # Calculate profit for buying materials in Martlock and selling in Thetford or Brecilien
    > price Adept's Bag --sell Thetford Brecilien --buy Martlock
)" << '\n';
}

void HelpCommand::printConfigHelp() const {
    std::cout << R"(
--- Help: setconfig ---
Manage application settings and preferences.

USAGE:
    > setconfig <setting_name> <value>

SETTINGS:
    database_path    (string) The path to the local item database file.
                              [Default: {source_directory}/database/aopc_items.db]

    market_tax       (float)  Market tax rate applied to sell price.
                              [Default: 0.08] (e.g., 0.08 for non-premium)

    region           (string) Target server region for API requests.
                              Valid: americas, europe, asia
                              [Default: europe]

    resource_return  (float)  Expected resource return rate percentage.
                              [Default: 0.153] (e.g., 0.153 for 15.3%)

    station_fee      (int)    Crafting station usage fee in silver.
                              CURRENTLY AN UNUSED SETTING.
                              [Default: 500]

EXAMPLES:
    # Set market tax on sold items to 8%
    > setconfig market_tax 0.08

    # Set regoin for checking prices to europe
    > setconfig region europe
)";
}