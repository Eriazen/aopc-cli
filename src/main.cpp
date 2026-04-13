#include <iostream>
#include "aopc-cli/core/commandHandler.hpp"
#include "aopc-cli/core/settings.hpp"
#include "aopc-cli/io/formatter.hpp"


CommandHandler handler;
Settings& settings = Settings::getInstance();

int main(int, char** argv){
    // Enable ANSI color codes on windows
    enableVirtualTerminal();

    // Initialize settings and command handler
    std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    std::filesystem::path dbPath = exeDir / "database" / "aopc_items.db";
    std::filesystem::path configPath = exeDir / "config" / "general.json";
    std::filesystem::path historyPath = exeDir / "history.txt";

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
    std::cout << "Albion Online Profit Calculator CLI tool started!" << std::endl;
    std::cout << "Type 'help' for a list of available commands.\n" << std::endl;

    // Start the command handler loop
    handler.run();

    return 0;
}