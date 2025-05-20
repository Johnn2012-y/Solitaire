#include "utils/Console.hpp"
#include <conio.h>
#include <iostream>

namespace utils {

HANDLE Console::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD Console::originalMode = 0;
bool Console::initialized = false;

void Console::init() {
    if (!initialized) {
        GetConsoleMode(hConsole, &originalMode);
        SetConsoleMode(hConsole, originalMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        initialized = true;
    }
}

void Console::clearScreen() {
    std::cout << "\x1B[2J\x1B[H";
}

void Console::setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

void Console::hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void Console::showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void Console::setTextColor(int color) {
    SetConsoleTextAttribute(hConsole, color);
}

void Console::setBackgroundColor(int color) {
    SetConsoleTextAttribute(hConsole, color << 4);
}

void Console::resetColors() {
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int Console::getScreenWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int Console::getScreenHeight() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Console::write(const std::string& text) {
    std::cout << text;
}

void Console::writeLine(const std::string& text) {
    std::cout << text << std::endl;
}

void Console::writeWide(const std::wstring& text) {
    DWORD written = 0;
    WriteConsoleW(hConsole, text.c_str(), static_cast<DWORD>(text.length()), &written, nullptr);
}

void Console::writeWideLine(const std::wstring& text) {
    writeWide(text);
    writeWide(L"\n");
}

char Console::getChar() {
    return _getch();
}

std::string Console::getLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

} // namespace utils 