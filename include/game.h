#ifndef GAME_H
#define GAME_H

#include "game_components.h"
#include "commands.h"
#include <vector>
#include <string>
#include <chrono>

class Game {
private:
    size_t wasteWindowStart = 0;
    size_t wasteWindowSize = 0;
    Stock stock;
    Waste waste;
    std::vector<Foundation> foundations;
    std::vector<Tableau> tableaus;
    std::vector<Command*> undoStack;
    int moveCount;
    int drawNum;
    int numTableaus;
    bool largeCards;
    bool polish;
    std::chrono::steady_clock::time_point gameStartTime;

    void clearUndoStack();
    std::wstring getCardDisplay(const Card& card) const;
    std::wstring getEmptyCardDisplay() const;

public:
    Game(int numTab, int drawN, bool largeCards = false, bool polish = true);
    ~Game();
    void setup();
    void display() const;
    void play();
    bool checkWin() const;
    int getMoveCount() const { return moveCount; }
    void showInstructions(bool isEnglish = false) const;

    friend class DrawCommand;
    friend class MoveCardCommand;
    friend class MoveSequenceCommand;
};

#endif // GAME_H 