#include "aopc-cli/io/formatter.hpp"
#include "aopc-cli/core/constants.hpp"
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

    void printIntroMessage() {
        std::cout    
            << constants::C_HL1 << "\x1b[4mA\x1b[24m" << constants::C_TEXT << "lbion "
            << constants::C_HL1 << "\x1b[4mO\x1b[24m" << constants::C_TEXT << "nline "
            << constants::C_HL1 << "\x1b[4mP\x1b[24m" << constants::C_TEXT << "rofit "
            << constants::C_HL1 << "\x1b[4mC\x1b[24m" << constants::C_TEXT << "alculator CLI tool "
            << "version " << project::VERSION << " started!\n"

            << "Type '" << constants::C_HL1 << "help" << constants::C_TEXT 
            << "' for a list of available commands.\n\n";
    }
}