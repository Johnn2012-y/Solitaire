#pragma once

#include "game/GameManager.hpp"
#include "utils/Color.hpp"
#include "utils/Console.hpp"
#include <string>

namespace ui {

class UI {
public:
    UI();
    
    void displayGame(const game::GameManager& game) const;
    void displayMessage(const std::string& message) const;
    void displayError(const std::string& error) const;
    void displayWin(int moves) const;
    void clearScreen() const;
    
    std::string getInput() const;
    char getChar() const;
    
    void setLanguage(bool polish);
    bool isPolish() const;

private:
    bool polish;
    
    void displayFoundations(const game::GameManager& game) const;
    void displayTableaus(const game::GameManager& game) const;
    void displayWaste(const game::GameManager& game) const;
    void displayCommands() const;
    
    std::wstring getCardString(const card::Card& card) const;
    std::string getLocalizedString(const std::string& pl, const std::string& en) const;
};

} // namespace ui 