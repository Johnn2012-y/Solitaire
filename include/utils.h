#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <windows.h>

// Color constants
extern const std::string RESET;
extern const char* BLACK;
extern const char* RED;
extern const char* GREEN;
extern const char* YELLOW;
extern const char* BLUE;
extern const char* MAGENTA;
extern const char* CYAN;
extern const char* WHITE;
extern const char* BOLD;
extern const char* DIM;
extern const char* UNDERLINE;
extern const char* BLINK;
extern const char* REVERSE;
extern const char* HIDDEN;
extern const char* YELLOW_BG;
extern const char* BLUE_BG;
extern const char* MAGENTA_BG;
extern const char* CYAN_BG;
extern const char* BRIGHT_RED;
extern const char* BRIGHT_YELLOW;
extern const char* BRIGHT_GREEN;
extern const char* BRIGHT_CYAN;
extern const char* BRIGHT_MAG;

// Utility functions
inline void clearScreen() {
    std::cout << "\x1B[2J\x1B[H";
}

#endif // UTILS_H 