#pragma once

#include <string>
#include <windows.h>

namespace utils {

class Console {
public:
    static void init();
    static void clearScreen();
    static void setCursorPosition(int x, int y);
    static void hideCursor();
    static void showCursor();
    static void setTextColor(int color);
    static void setBackgroundColor(int color);
    static void resetColors();
    static int getScreenWidth();
    static int getScreenHeight();
    static void write(const std::string& text);
    static void writeLine(const std::string& text);
    static void writeWide(const std::wstring& text);
    static void writeWideLine(const std::wstring& text);
    static char getChar();
    static std::string getLine();

private:
    static HANDLE hConsole;
    static DWORD originalMode;
    static bool initialized;
};

} // namespace utils 