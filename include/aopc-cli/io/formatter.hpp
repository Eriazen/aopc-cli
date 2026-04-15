#pragma once

#include <string>
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif


namespace TextFormatter {
    std::string formatNumber(int value);
    std::string formatCurrency(int value, std::string currency);
    std::string formatPercentage(float value, int precision = 1);

    template <typename Container>
    void printWrappedList(const Container& items, const std::string& indent, int maxWidth) {
        int currentLineLength = static_cast<int>(indent.length());

        for (auto it = std::cbegin(items); it != std::cend(items); ++it) {
            bool isLast = (std::next(it) == std::cend(items));
            std::string word = std::string(*it);
            if (isLast) {
                word += "";
            } else {
                word += ", ";
            }

            if (currentLineLength + static_cast<int>(word.length()) > maxWidth) {
                std::cout << '\n' << indent;
                currentLineLength = static_cast<int>(indent.length());
            }

            std::cout << word;
            currentLineLength += static_cast<int>(word.length());
        }
    }
}

inline void enableVirtualTerminal() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    system("color 07");
#endif
}

inline void setUpConsole() { 
#ifdef _WIN32
    SetConsoleTitleA("Albion Online Price Checker");
#endif
}