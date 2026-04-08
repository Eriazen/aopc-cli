#include <iostream>
#include "aopc-cli/core/commandHandler.hpp"
#include "aopc-cli/core/settings.hpp"


CommandHandler handler;
Settings& settings = Settings::getInstance();

int main(int, char** argv){
    // Initialize settings and command handler
    std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
    std::filesystem::path dbPath = exeDir / "database" / "aopc_items.db";
    std::filesystem::path settingsPath = exeDir / "config" / "settings.json";

    // Load settings from file (if it exists)
    if (!settings.loadSettingsFromFile(settingsPath)) {
        settings.setDatabasePath(dbPath);
        settings.setRegionURL("europe");
        // Save default settings to file
        settings.saveSettingsToFile(settingsPath);
    }

    // Print welcome message
    std::cout << "Albion Online Price Checker CLI tool started!" << std::endl;
    std::cout << "Type 'help' for a list of available commands.\n" << std::endl;

    // Start the command handler loop
    handler.run();

    return 0;
}