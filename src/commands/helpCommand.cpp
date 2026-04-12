#include "aopc-cli/commands/helpCommand.hpp"
#include "aopc-cli/io/argParser.hpp"
#include "aopc-cli/core/constants.hpp"
#include "aopc-cli/io/formatter.hpp"
#include <iostream>
#include <sstream>


HelpCommand::HelpCommand() {
    m_commands["price"] = [this]() { return printPriceHelp(); };
    m_commands["config"] = [this]() { return printConfigHelp(); };
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
    config      Manage application settings and preferences.
    help        Show this help message.
    exit        Close the application.

Type 'help <command>' for detailed information.
)" << '\n';
}

void HelpCommand::printPriceHelp() const {
    std::cout << R"(
--- Help: price ---
Check the market price and calculate profit for crafting an item.

USAGE:
    > price <item_name> [--cities <c1> <c2>...] [--qualities <q1> <q2>...]

OPTIONS:
    --cities        Specify target cities for the price check.
                    Valid: )";

    std::string lineIndent = "                           ";
    TextFormatter::printWrappedList(constants::CITIES, lineIndent, constants::TOTAL_LINE_WIDTH);

    std::cout << R"(
                    [Default: all citites]

    --qualities     Specify item qualities using codes 1-5.
                    1 - Normal, 2 - Good, 3 - Oustanding, 4 - Excellent, 5 - Masterpiece.
                    To fetch material prices, quality '1' must be present.
                    [Default: all cities]

EXAMPLES:
    > price Adept's Bag --cities Thetforde Brecilien --qualities 1 2
    > price Exceptional Ashenbark Planks 
)" << '\n';
}

void HelpCommand::printConfigHelp() const {
    std::cout << R"(
--- Help: config ---
Manage application settings and preferences.

USAGE:
    > config set <setting_name> <value>

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
    > config set market_tax 0.08
    > config set region europe
)";
}