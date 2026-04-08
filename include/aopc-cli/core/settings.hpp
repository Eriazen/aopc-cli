#pragma once

#include <string>
#include <filesystem>
#include "aopc-cli/core/constants.hpp"


class Settings {
    public:
        static Settings& getInstance();

        Settings(const Settings&) = delete;
        Settings& operator=(const Settings&) = delete;

        std::filesystem::path getDatabasePath() const;
        std::string getRegionURL() const;

        bool setDatabasePath(const std::filesystem::path& path);
        bool setRegionURL(const std::string& keyword);

        bool loadSettingsFromFile(const std::filesystem::path& filePath);
        bool saveSettingsToFile(const std::filesystem::path& filePath) const;

    private:
        Settings() = default;
        ~Settings() = default;

        std::string regionURL;
        std::filesystem::path databasePath;
};