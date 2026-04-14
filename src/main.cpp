#include <iostream>
#include "aopc-cli/core/commandHandler.hpp"
#include "aopc-cli/core/settings.hpp"
#include "aopc-cli/io/formatter.hpp"
#include "aopc-cli/core/constants.hpp"


CommandHandler handler;
Settings& settings = Settings::getInstance();

int main(int, char** argv){
    // Enable ANSI color codes on windows
    enableVirtualTerminal();
    setUpConsole();

    // Initialize settings and command handler
    std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    std::filesystem::path dbPath = exeDir / "database" / "aopc_items.db";
    std::filesystem::path configPath = exeDir / "config" / "general.json";
    std::filesystem::path historyPath = exeDir / "command_history.txt";

    settings.setConfigPath(configPath);

    // Load settings from file (if it exists)
    if (!settings.loadSettingsFromFile()) {
        settings.setDatabasePath(dbPath);
        settings.setRegionURL("europe");
        settings.setMarketTax(0.08f);
        settings.setResourceReturnRate(0.153f);
        settings.setStationFee(500);
        // Save default settings to file
        settings.saveSettingsToFile();
    }

    ic_set_history(historyPath.string().c_str(), 200);

    // Print welcome message
    std::cout    
        << constants::C_HL1 << "\x1b[4mA\x1b[24m" << constants::C_TEXT << "lbion "
        << constants::C_HL1 << "\x1b[4mO\x1b[24m" << constants::C_TEXT << "nline "
        << constants::C_HL1 << "\x1b[4mP\x1b[24m" << constants::C_TEXT << "rofit "
        << constants::C_HL1 << "\x1b[4mC\x1b[24m" << constants::C_TEXT << "alculator CLI tool started!\n"

        << "Type '" << constants::C_HL1 << "help" << constants::C_TEXT 
        << "' for a list of available commands.\n\n";

    std::cout << constants::C_HINT << "hint" << constants::C_RESET << '\n';
    std::cout << constants::C_HL1 << "highlight earth" << constants::C_RESET << '\n';
    std::cout << constants::C_HL2 << "highlight sky" << constants::C_RESET << '\n';
    std::cout << constants::C_HL3 << "highlight slate" << constants::C_RESET << '\n';
    std::cout << constants::C_SUCCESS << "success" << constants::C_RESET << '\n';
    std::cout << constants::C_ERROR << "error" << constants::C_RESET << '\n';
    std::cout << constants::C_TEXT << "text" << constants::C_RESET << '\n';

    // Start the command handler loop
    handler.run();

    return 0;
}