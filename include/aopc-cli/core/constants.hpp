#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include <vector>
#include <array>


// Namespace to hold constant values for the application
namespace constants {
    // API endpoints
    inline constexpr std::string_view API_EUROPE_URL { "https://europe.albion-online-data.com" };
    inline constexpr std::string_view API_AMERICAS_URL { "https://west.albion-online-data.com" };
    inline constexpr std::string_view API_ASIA_URL { "https://east.albion-online-data.com" };
    inline constexpr std::string_view API_PRICE_ENDPOINT { "/api/v2/stats/prices" };
    inline constexpr std::string_view API_GOLD_ENDPOINT { "/api/v2/stats/gold.json?" };

    // Albion Online values
    inline constexpr std::array<std::string_view, 7> CITIES { "Caerleon", "Lymhurst", "Bridgewater", "Martlock", "Thetford", "Fort Sterling", "Brecilien" };
    inline constexpr std::array<std::string_view, 5> ITEM_QUALITIES { "Normal", "Good", "Outstanding", "Excellent", "Masterpiece" };
}

#endif // CONSTANTS_HPP