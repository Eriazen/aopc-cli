#pragma once

#include <string>
#include <vector>
#include <array>


// Namespace to hold constant values for the application
namespace constants {
    // API endpoints
    inline constexpr std::string_view API_EUROPE_URL { "https://europe.albion-online-data.com" };
    inline constexpr std::string_view API_AMERICAS_URL { "https://west.albion-online-data.com" };
    inline constexpr std::string_view API_ASIA_URL { "https://east.albion-online-data.com" };
    inline constexpr std::string_view API_PRICE_ENDPOINT { "/api/v2/stats/prices/" };
    inline constexpr std::string_view API_GOLD_ENDPOINT { "/api/v2/stats/gold.json?" };

    // Albion Online values
    inline constexpr std::array<std::string_view, 8> CITIES { "Caerleon", "Lymhurst", "Bridgewatch", "Martlock", "Thetford", "FortSterling", "Brecilien", "BlackMarket" };
    //inline constexpr std::array<std::string_view, 5> ITEM_QUALITIES { "Normal", "Good", "Outstanding", "Excellent", "Masterpiece" };
    //inline constexpr std::array<std::string_view, 5> ITEM_QUALITIES { "1", "2", "3", "4", "5" };
    inline constexpr std::array<int, 5> ITEM_QUALITIES { 1, 2, 3, 4, 5 };

    // Output formatting
    inline constexpr unsigned int TOTAL_LINE_WIDTH { 100 };
    inline constexpr std::string_view C_RESET { "\x1b[0m" };
    inline constexpr std::string_view C_HINT { "\x1b[38;5;243m" };
    inline constexpr std::string_view C_HL1 { "\x1b[38;5;180m" };
    inline constexpr std::string_view C_HL2 { "\x1b[38;5;110m" };
    inline constexpr std::string_view C_HL3 { "\x1b[38;5;103m" };
    inline constexpr std::string_view C_SUCCESS { "\x1b[38;5;108m" };
    inline constexpr std::string_view C_ERROR { "\x1b[38;5;131m" };
    inline constexpr std::string_view C_TEXT { "\x1b[38;5;247m" };
}