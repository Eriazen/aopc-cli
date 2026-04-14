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
        std::cout << "Type '"
            << constants::C_HL1 << "help" << constants::C_RESET
            << "' for a list of available commands." << '\n';
    }
}

void HelpCommand::printGeneralHelp() const {
    std::cout 
        << constants::C_TEXT << "====================================================================================================\n"
        << constants::C_HL2     << " Albion Online Profit Calculator - Help Menu\n"
        << constants::C_TEXT << "====================================================================================================\n\n"
        
        << constants::C_HL2     << "USAGE:\n"
        << constants::C_TEXT << "    > " << constants::C_HL1 << "<command> " << constants::C_HL3 << "[arguments] [flags]\n\n"
        
        << constants::C_HL2     << "COMMANDS:\n"
        << constants::C_HL1     << "    price       " << constants::C_TEXT << "Check the market price and calculate profit for crafting an item.\n"
        << constants::C_HL1     << "    setconfig   " << constants::C_TEXT << "Manage application settings and preferences.\n"
        << constants::C_HL1     << "    help        " << constants::C_TEXT << "Show this help message.\n"
        << constants::C_HL1     << "    exit        " << constants::C_TEXT << "Close the application.\n\n"
        
        << constants::C_TEXT << "Type '" << constants::C_HL1 << "help " << constants::C_HL3 << "<command>" << constants::C_TEXT << "' for detailed information.\n"
        << constants::C_RESET;
}

void HelpCommand::printPriceHelp() const {
    std::cout 
        << constants::C_TEXT << "====================================================================================================\n"
        << constants::C_HL2     << " Albion Online Profit Calculator - Price Command\n"
        << constants::C_TEXT << "====================================================================================================\n\n"
        
        << constants::C_TEXT << "Calculates crafting profitability and cross-references market prices across multiple cities.\n\n"
        
        << constants::C_HL2     << "USAGE:\n"
        << constants::C_TEXT << "    > " << constants::C_HL1 << "price " << constants::C_HL3 << "<item_name> " 
        << constants::C_TEXT << "[" << constants::C_HL1 << "--buy " << constants::C_HL3 << "<city>..." << constants::C_TEXT << "] "
        << "[" << constants::C_HL1 << "--sell " << constants::C_HL3 << "<city>..." << constants::C_TEXT << "]\n\n"
        
        << constants::C_HL2     << "OPTIONS:\n"
        << constants::C_HL1     << "    --buy " << constants::C_HL3 << "<cities>  " 
        << constants::C_TEXT << "Calculate material crafting costs sourced from these cities.\n"
        << "                    If omitted, the command skips materials and runs in \"Sell-Only\" mode.\n"
        << "                    " << constants::C_HINT << "[Default: none]\n\n"
        
        << constants::C_HL1     << "    --sell " << constants::C_HL3 << "<cities> " 
        << constants::C_TEXT << "Check final item market prices and profit margins in these target cities.\n"
        << "                    " << constants::C_HINT << "[Default: all cities]\n\n"
        
        << constants::C_HL2     << "VALID CITIES:\n"
        << constants::C_TEXT;

    // Print the wrapped list with a clean 4-space indent under the new header
    std::string lineIndent = "    ";
    TextFormatter::printWrappedList(constants::CITIES, lineIndent, constants::TOTAL_LINE_WIDTH);

    std::cout 
        << "\n\n"
        << constants::C_HL2     << "EXAMPLES:\n"
        << constants::C_HINT    << "    # Check sell prices for planks across all cities (Sell-Only mode)\n"
        << constants::C_TEXT << "    > " << constants::C_HL1 << "price " << constants::C_TEXT << "\"Exceptional Ashenbark Planks\"\n\n"
        
        << constants::C_HINT    << "    # Calculate profit for buying materials in Martlock and selling in Thetford or Brecilien\n"
        << constants::C_TEXT << "    > " << constants::C_HL1 << "price " << constants::C_TEXT << "\"Adept's Bag\" " 
        << constants::C_HL1     << "--buy " << constants::C_TEXT << "Martlock " 
        << constants::C_HL1     << "--sell " << constants::C_TEXT << "Thetford Brecilien\n"
        << constants::C_RESET   << '\n';
}

void HelpCommand::printConfigHelp() const {
    std::cout 
        << constants::C_TEXT << "====================================================================================================\n"
        << constants::C_HL2     << " Albion Online Profit Calculator - Config Command\n"
        << constants::C_TEXT << "====================================================================================================\n\n"
        
        << constants::C_TEXT << "Manage application settings and preferences.\n\n"
        
        << constants::C_HL2     << "USAGE:\n"
        << constants::C_TEXT << "    > " << constants::C_HL1 << "setconfig " 
        << constants::C_HL3     << "<setting_name> <value>\n\n"
        
        << constants::C_HL2     << "SETTINGS:\n"
        
        << constants::C_HL1     << "    database_path   " << constants::C_TEXT << "(string)  " 
        << constants::C_TEXT << "The path to the local item database file.\n"
        << "                              " << constants::C_HINT << "[Default: {source_directory}/database/aopc_items.db]\n\n"
        
        << constants::C_HL1     << "    market_tax      " << constants::C_TEXT << "(float)   " 
        << constants::C_TEXT << "Market tax rate applied to sell price.\n"
        << "                              " << constants::C_HINT << "[Default: 0.08] (e.g., 0.08 for non-premium)\n\n"
        
        << constants::C_HL1     << "    region          " << constants::C_TEXT << "(string)  " 
        << constants::C_TEXT << "Target server region for API requests.\n"
        << "                              " << constants::C_TEXT << "Valid: americas, europe, asia\n"
        << "                              " << constants::C_HINT << "[Default: europe]\n\n"
        
        << constants::C_HL1     << "    resource_return " << constants::C_TEXT << "(float)   " 
        << constants::C_TEXT << "Expected resource return rate percentage.\n"
        << "                              " << constants::C_HINT << "[Default: 0.153] (e.g., 0.153 for 15.3%)\n\n"
        
        << constants::C_HL1     << "    station_fee     " << constants::C_TEXT << "(int)     " 
        << constants::C_TEXT << "Crafting station usage fee in silver.\n"
        << "                              " << constants::C_ERROR << "CURRENTLY AN UNUSED SETTING.\n"
        << "                              " << constants::C_HINT << "[Default: 500]\n\n"

        << constants::C_HL2     << "EXAMPLES:\n"
        << constants::C_HINT    << "    # Set market tax on sold items to 8%\n"
        << constants::C_TEXT << "    > " << constants::C_HL1 << "setconfig " << constants::C_TEXT << "market_tax 0.08\n\n"
        
        << constants::C_HINT    << "    # Set region for checking prices to europe\n"
        << constants::C_TEXT << "    > " << constants::C_HL1 << "setconfig " << constants::C_TEXT << "region europe\n"
        << constants::C_RESET   << '\n';
}