#pragma once

#include "ui/UI.hpp"
#include "game/GameManager.hpp"
#include <memory>

namespace ui {

class Menu {
public:
    Menu();
    void run();

private:
    UI ui;
    bool easy;
    bool small;
    bool polish;
    
    void showMainMenu() const;
    void showSettings() const;
    void showHowToPlay() const;
    void showRanking() const;
    void startGame();
    void toggleLanguage();
    void animateIntro() const;
    void animateBackground(int drops) const;
};

} // namespace ui 