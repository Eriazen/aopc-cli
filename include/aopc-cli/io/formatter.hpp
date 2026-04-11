#pragma once

#include <string>


namespace TextFormatter {
    std::string formatNumber(int value);
    std::string formatCurrency(int value, std::string currency);
    std::string formatPercentage(float value, int precision = 1);
}