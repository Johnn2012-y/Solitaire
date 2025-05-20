#include "../include/menu.h"
#include "../include/utils.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>

void Ranking::saveScore(const std::string& difficulty, const std::string& size, int moves) {
    std::ofstream file("ranking.txt", std::ios::app);
    file << difficulty << " " << size << " " << moves << "\n";
}

void Ranking::displayRanking() const {
    std::ifstream file("ranking.txt");
    std::string line;
    while (std::getline(file, line)) std::cout << line << "\n";
}

void Menu::initConsole() {
    SetConsoleOutputCP(CP_UTF8);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0; GetConsoleMode(h, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(h, mode);
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
}

void Menu::animateIntro() {
    std::string title = polish ? " PASJANSOWE WYZWANIE " : " SOLITAIRE QUEST ";
    const char* cols[] = {BRIGHT_RED, BRIGHT_YELLOW, BRIGHT_GREEN, BRIGHT_CYAN, BRIGHT_MAG};
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i < (int)title.size(); ++i) {
            clearScreen(); std::cout << BOLD;
            for (int j = 0; j < (int)title.size(); ++j) std::cout << cols[(i+j)%5] << title[j];
            std::cout << RESET << "\n";
            Sleep(70);
        }
    }
    Sleep(300);
}

void Menu::animateBackground(int drops) {
    clearScreen();
    int width = 32, height = 16;
    std::vector<std::pair<int,int>> pts;
    for (int i = 0; i < drops; ++i)
        pts.emplace_back(rand() % height, rand() % width);
    const char* suits[] = {u8"♠", u8"♣", u8"♥", u8"♦"};
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool drawn = false;
            for (auto& p : pts) if (p.first==y && p.second==x) {
                int idx = rand()%4;
                const char* color = (idx>=2?BRIGHT_RED:BRIGHT_CYAN);
                std::cout << BOLD << color << suits[idx] << RESET;
                drawn=true; Sleep(10); break;
            }
            if (!drawn) std::cout << ' ';
        }
        std::cout << "\n";
    }
}

void Menu::showMainMenu() {
    clearScreen();
    if (polish) {
        std::cout << BRIGHT_CYAN
                 << "+=============================+\n"
                 << "|       " << BRIGHT_YELLOW << "SOLITAIRE QUEST" << BRIGHT_CYAN << "       |\n"
                 << "+=============================+" << RESET << "\n\n"
                 << GREEN  << "      [1] ROZPOCZNIJ GRĘ      " << "\n\n" << RESET
                 << YELLOW << "      [2] POKAŻ RANKING       " << "\n\n" << RESET
                 << MAGENTA << "      [3] JAK GRAĆ            " << "\n\n" << RESET
                 << CYAN   << "      [4] JĘZYK (PL/EN)       " << "\n\n" << RESET
                 << RED    << "      [5] WYJŚCIE             " << RESET << "\n\n";
        std::cout << BRIGHT_YELLOW << "      WCIŚNIJ 1–5" << RESET << "\n";
    } else {
        std::cout << BRIGHT_CYAN
                 << "+=============================+\n"
                 << "|       " << BRIGHT_YELLOW << "SOLITAIRE QUEST" << BRIGHT_CYAN << "       |\n"
                 << "+=============================+" << RESET << "\n\n"
                 << GREEN  << "      [1] START GAME          " << "\n\n" << RESET
                 << YELLOW << "      [2] SHOW RANKING        " << "\n\n" << RESET
                 << MAGENTA << "      [3] HOW TO PLAY         " << "\n\n" << RESET
                 << CYAN   << "      [4] LANGUAGE (PL/EN)    " << "\n\n" << RESET
                 << RED    << "      [5] EXIT                " << RESET << "\n\n";
        std::cout << BRIGHT_YELLOW << "      PRESS 1–5" << RESET << "\n";
    }
}

void Menu::toggleLanguage() {
    polish = !polish;
}

void Menu::showHowToPlay() {
    Game g(7, 1, false, polish); // Create temporary Game instance just for showing instructions
    g.showInstructions(!polish); // Pass !polish to show English if polish is false
}

void Menu::showRanking() {
    clearScreen();
    std::cout << BOLD << BRIGHT_MAG << (polish?"~~~~~ RANKING ~~~~~":"~~~~~ RANKING ~~~~~") << RESET << "\n\n";
    ranking.displayRanking();
    std::cout << "\n" << BRIGHT_GREEN << (polish?"[dowolny klawisz]":"[any key]") << RESET << " back\n";
    _getch();
}

void Menu::startGame() {
    while (true) {
        clearScreen();

        if (polish) {
            std::cout << BOLD << BRIGHT_GREEN
                     << "+--------- Ustawienia ---------+" << RESET << "\n\n";
            std::cout << "  Poziom trudności: ";
            std::cout << (easy?std::string(BRIGHT_GREEN)+"ŁATWY":std::string(BRIGHT_RED)+"TRUDNY") << RESET << "  (1)\n";

            std::cout << "  Układ:           ";
            std::cout << (smalll?std::string(BRIGHT_GREEN)+"MAŁY":std::string(BRIGHT_YELLOW)+"DUŻY") << RESET << "  (2)\n\n";

            std::cout << "  Rozmiar kart:    ";
            std::cout << (largeCards?std::string(BRIGHT_YELLOW)+"DUŻY":std::string(BRIGHT_GREEN)+"MAŁY") << RESET << "  (3)\n\n";

            std::cout << BRIGHT_CYAN << "  [4] GRAJ TERAZ!  " << RESET << "\n\n";
            std::cout << BOLD << BRIGHT_GREEN << "  Użyj 1/2/3 by przełączyć, 4 by grać" << RESET << "\n\n";
        } else {
            std::cout << BOLD << BRIGHT_GREEN
                     << "+--------- SETTINGS ---------+" << RESET << "\n\n";
            std::cout << "  Difficulty:      ";
            std::cout << (easy?std::string(BRIGHT_GREEN)+"EASY":std::string(BRIGHT_RED)+"HARD") << RESET << "  (1)\n";

            std::cout << "  Layout:          ";
            std::cout << (smalll?std::string(BRIGHT_GREEN)+"SMALL":std::string(BRIGHT_YELLOW)+"LARGE") << RESET << "  (2)\n\n";

            std::cout << "  Card Size:       ";
            std::cout << (largeCards?std::string(BRIGHT_YELLOW)+"LARGE":std::string(BRIGHT_GREEN)+"SMALL") << RESET << "  (3)\n\n";

            std::cout << BRIGHT_CYAN << "  [4] PLAY NOW!   " << RESET << "\n\n";
            std::cout << BOLD << BRIGHT_GREEN << "  Use 1/2/3 to toggle, 4 to play" << RESET << "\n\n";
        }
        char c = _getch();
        if (c=='1') easy = !easy;
        else if(c=='2') smalll = !smalll;
        else if(c=='3') largeCards = !largeCards;
        else if(c=='4') {
            int nt = smalll?4:7;
            int dn = easy?1:3;
            Game g(nt, dn, largeCards, polish);
            g.play();
            if(g.checkWin()) {
                ranking.saveScore(
                    polish?(easy?"łatwy":"trudny"):(easy?"easy":"hard"),
                    polish?(smalll?"mały":"duży"):(smalll?"small":"large"),
                    g.getMoveCount()
                );
            }
            break;
        }
    }
}

void Menu::run() {
    initConsole();
    srand((unsigned)time(nullptr));
    
    // Ask for language first
    clearScreen();
    std::cout << BRIGHT_CYAN
              << "+=========================================+\n"
              << "|     " << BRIGHT_YELLOW << "WYBIERZ JĘZYK / CHOOSE LANGUAGE" << BRIGHT_CYAN << "     |\n"
              << "+=========================================+\n"
              << GREEN  << "      [1] POLSKI / POLISH      " << "\n\n" << RESET
              << YELLOW << "      [2] ENGLISH / ANGIELSKI   " << RESET << "\n\n";
    
    while (true) {
        char c = _getch();
        if (c == '1') {
            polish = true;
            break;
        } else if (c == '2') {
            polish = false;
            break;
        }
    }
    
    animateIntro();
    while (true) {
        animateBackground(40);
        showMainMenu();
        char c = _getch();
        switch (c) {
            case '1': startGame();      break;
            case '2': showRanking();    break;
            case '5': return;           // Exit
            case '3': showHowToPlay();  break;
            case '4': toggleLanguage(); break;
            default: break;
        }
    }
} 