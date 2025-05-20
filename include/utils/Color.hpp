#pragma once

#include <string>

namespace utils {

class Color {
public:
    static const std::string RESET;
    static const std::string BLACK;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string MAGENTA;
    static const std::string CYAN;
    static const std::string WHITE;
    static const std::string BOLD;
    static const std::string DIM;
    static const std::string UNDERLINE;
    static const std::string BLINK;
    static const std::string REVERSE;
    static const std::string HIDDEN;
    static const std::string YELLOW_BG;
    static const std::string BLUE_BG;
    static const std::string MAGENTA_BG;
    static const std::string CYAN_BG;
    static const std::string BRIGHT_RED;
    static const std::string BRIGHT_YELLOW;
    static const std::string BRIGHT_GREEN;
    static const std::string BRIGHT_CYAN;
    static const std::string BRIGHT_MAG;

    // Helper methods for card colors
    static const std::string& getCardColor(bool isRed);
    static const std::string& getSuitColor(int suit);
};

} // namespace utils 