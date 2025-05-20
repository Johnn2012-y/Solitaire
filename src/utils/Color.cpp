#include "utils/Color.hpp"

namespace utils {

const std::string Color::RESET = "\033[0m";
const std::string Color::BLACK = "\x1B[30m";
const std::string Color::RED = "\x1B[31m";
const std::string Color::GREEN = "\x1B[32m";
const std::string Color::YELLOW = "\x1B[33m";
const std::string Color::BLUE = "\x1B[34m";
const std::string Color::MAGENTA = "\x1B[35m";
const std::string Color::CYAN = "\x1B[36m";
const std::string Color::WHITE = "\x1B[37m";
const std::string Color::BOLD = "\x1B[1m";
const std::string Color::DIM = "\x1B[2m";
const std::string Color::UNDERLINE = "\x1B[4m";
const std::string Color::BLINK = "\x1B[5m";
const std::string Color::REVERSE = "\x1B[7m";
const std::string Color::HIDDEN = "\x1B[8m";
const std::string Color::YELLOW_BG = "\x1B[43m\x1B[30m";
const std::string Color::BLUE_BG = "\x1B[44m\x1B[97m";
const std::string Color::MAGENTA_BG = "\x1B[45m\x1B[97m";
const std::string Color::CYAN_BG = "\x1B[46m\x1B[30m";
const std::string Color::BRIGHT_RED = "\x1B[91m";
const std::string Color::BRIGHT_YELLOW = "\x1B[93m";
const std::string Color::BRIGHT_GREEN = "\x1B[92m";
const std::string Color::BRIGHT_CYAN = "\x1B[96m";
const std::string Color::BRIGHT_MAG = "\x1B[95m";

const std::string& Color::getCardColor(bool isRed) {
    return isRed ? BRIGHT_RED : WHITE;
}

const std::string& Color::getSuitColor(int suit) {
    // Hearts (0) and Diamonds (1) are red, Clubs (2) and Spades (3) are black
    return (suit < 2) ? BRIGHT_RED : WHITE;
}

} // namespace utils 