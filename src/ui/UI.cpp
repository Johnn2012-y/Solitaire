#include "ui/UI.hpp"
#include <sstream>
#include <algorithm>

namespace ui {

UI::UI() : polish(true) {}

void UI::displayGame(const game::GameManager& game) const {
    clearScreen();
    displayFoundations(game);
    displayTableaus(game);
    displayWaste(game);
    displayCommands();
}

void UI::displayFoundations(const game::GameManager& game) const {
    utils::Console::writeLine(getLocalizedString("Fundacja:", "Foundation:"));
    
    static const std::wstring foundationSuits[] = {L"♥", L"♦", L"♣", L"♠"};
    for (size_t i = 0; i < game.getFoundations().size(); ++i) {
        std::wstringstream ss;
        ss << L"F" << (i + 1) << foundationSuits[i] << L": ";
        
        const auto& foundation = game.getFoundations()[i];
        if (!foundation.empty()) {
            const auto& card = foundation.getTop();
            ss << utils::Color::getCardColor(card.isRed()) << getCardString(card) << utils::Color::RESET;
        }
        
        utils::Console::writeWideLine(ss.str());
    }
    utils::Console::writeLine("");
}

void UI::displayTableaus(const game::GameManager& game) const {
    utils::Console::writeLine(getLocalizedString("Tableau:", "Tableau:"));
    
    for (size_t i = 0; i < game.getTableaus().size(); ++i) {
        std::wstringstream ss;
        ss << L"T" << (i + 1) << L": ";
        
        const auto& tableau = game.getTableaus()[i];
        for (const auto& card : tableau.getFaceDown()) {
            ss << L"XX ";
        }
        for (const auto& card : tableau.getFaceUp()) {
            ss << utils::Color::getCardColor(card.isRed()) << getCardString(card) << L" " << utils::Color::RESET;
        }
        
        utils::Console::writeWideLine(ss.str());
    }
    utils::Console::writeLine("");
}

void UI::displayWaste(const game::GameManager& game) const {
    std::wstringstream ss;
    ss << getLocalizedString(L"Waste [", L"Waste [") << game.getWaste().size() << L"]: ";
    
    const auto& wasteCards = game.getWaste().getCards();
    size_t start = game.getWasteWindowStart();
    size_t end = start + game.getWasteWindowSize();
    if (end > wasteCards.size()) end = wasteCards.size();
    
    for (size_t i = start; i < end; ++i) {
        ss << utils::Color::getCardColor(wasteCards[i].isRed()) << getCardString(wasteCards[i]) << L" " << utils::Color::RESET;
    }
    
    utils::Console::writeWideLine(ss.str());
    utils::Console::writeLine("");
}

void UI::displayCommands() const {
    utils::Console::writeLine(getLocalizedString(
        "Komendy: d, m <skąd> <dokąd> [n], u, q, r",
        "Commands: d, m <from> <to> [n], u, q, r"
    ));
}

void UI::displayMessage(const std::string& message) const {
    utils::Console::writeLine(message);
}

void UI::displayError(const std::string& error) const {
    utils::Console::writeLine(utils::Color::BRIGHT_RED + error + utils::Color::RESET);
}

void UI::displayWin(int moves) const {
    std::stringstream ss;
    ss << getLocalizedString(
        "Wygrałeś w " + std::to_string(moves) + " ruchach! (n – nowa gra, q – wyjście)",
        "You won in " + std::to_string(moves) + " moves! (n – new game, q – quit)"
    );
    utils::Console::writeLine(ss.str());
}

void UI::clearScreen() const {
    utils::Console::clearScreen();
}

std::string UI::getInput() const {
    return utils::Console::getLine();
}

char UI::getChar() const {
    return utils::Console::getChar();
}

void UI::setLanguage(bool polish) {
    this->polish = polish;
}

bool UI::isPolish() const {
    return polish;
}

std::wstring UI::getCardString(const card::Card& card) const {
    return card.toWString();
}

std::string UI::getLocalizedString(const std::string& pl, const std::string& en) const {
    return polish ? pl : en;
}

} // namespace ui 