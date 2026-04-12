#pragma once

#include <string>
#include <filesystem>
#include "aopc-cli/core/constants.hpp"


class Settings {
    public:
        static Settings& getInstance();

        Settings(const Settings&) = delete;
        Settings& operator=(const Settings&) = delete;

        std::filesystem::path getConfigPath() const {return m_configPath; };
        std::filesystem::path getDatabasePath() const { return m_databasePath; };
        std::string getRegionURL() const { return m_regionURL; };
        float getMarketTax() const { return m_marketTax; };
        float getResourceReturnRate() const { return m_resourceReturnRate; };
        int getStationFee() const { return m_stationFee; };

        void setConfigPath(const std::filesystem::path& path);
        bool setDatabasePath(const std::filesystem::path& path);
        bool setRegionURL(const std::string& keyword);
        bool setMarketTax(const float& taxFraction);
        bool setResourceReturnRate(const float& rrrFraction);
        bool setStationFee(const int& stationFee);

        bool loadSettingsFromFile();
        bool saveSettingsToFile() const;

    private:
        Settings() = default;
        ~Settings() = default;

        std::filesystem::path m_configPath;
        std::string m_regionURL;
        std::filesystem::path m_databasePath;
        float m_marketTax;
        float m_resourceReturnRate;
        int m_stationFee;
};