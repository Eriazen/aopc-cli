#include "aopc-cli/io/formatter.hpp"
#include <sstream>
#include <iomanip>


namespace TextFormatter {
    std::string formatNumber(int value) {
        std::string str = std::to_string(value);

        int insertPosition = static_cast<int>(str.length()) - 3;
        while (insertPosition > 0) {
            if (insertPosition == 1 && str[0] == '-') {
                break;
            }

            str.insert(static_cast<size_t>(insertPosition), ",");
            insertPosition -= 3;
        }

        return str;
    }

    std::string formatCurrency(int value, std::string currency) {
        std::string str = std::to_string(value);

        int insertPosition = static_cast<int>(str.length()) - 3;
        while (insertPosition > 0) {
            if (insertPosition == 1 && str[0] == '-') {
                break;
            }

            str.insert(static_cast<size_t>(insertPosition), ",");
            insertPosition -= 3;
        }

        return str + currency;
    }

    std::string formatPercentage(float value, int precision) {
        std::ostringstream stream;

        stream << std::fixed << std::setprecision(precision) << value << "%";

        return stream.str();
    }
}