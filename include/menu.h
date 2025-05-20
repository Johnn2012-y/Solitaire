#ifndef MENU_H
#define MENU_H

#include "game.h"
#include <string>
#include <fstream>

class Ranking {
public:
    void saveScore(const std::string& difficulty, const std::string& size, int moves);
    void displayRanking() const;
};

class Menu {
public:
    void run();

private:
    bool easy = true;
    bool smalll = true;
    bool polish = true;
    bool largeCards = false;  // New option for card size
    Ranking ranking;

    void initConsole();
    void animateIntro();
    void animateBackground(int drops);
    void showMainMenu();
    void toggleLanguage();
    void showHowToPlay();
    void showRanking();
    void startGame();
};

#endif // MENU_H 