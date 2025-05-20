#pragma once

#include "game/Foundation.hpp"
#include "game/Tableau.hpp"
#include "game/Stock.hpp"
#include "game/Waste.hpp"
#include <vector>
#include <memory>

namespace game {

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual bool isExecuted() const { return true; }
};

class GameManager {
public:
    GameManager(int numTableaus, int drawCount);
    
    void setup();
    void play();
    bool checkWin() const;
    int getMoveCount() const;
    
    // Game actions
    void drawCards();
    bool moveCard(const std::string& from, const std::string& to);
    bool moveSequence(int fromIdx, int toIdx, size_t numCards);
    void undo();
    void restart();
    
    // Getters for UI
    const std::vector<Foundation>& getFoundations() const;
    const std::vector<Tableau>& getTableaus() const;
    const Stock& getStock() const;
    const Waste& getWaste() const;
    size_t getWasteWindowStart() const;
    size_t getWasteWindowSize() const;

private:
    std::vector<Foundation> foundations;
    std::vector<Tableau> tableaus;
    Stock stock;
    Waste waste;
    std::vector<std::unique_ptr<Command>> undoStack;
    int moveCount;
    int drawCount;
    int numTableaus;
    size_t wasteWindowStart;
    size_t wasteWindowSize;
    
    void clearUndoStack();
    bool validateMove(const std::string& from, const std::string& to) const;
    int parseIndex(const std::string& str) const;
};

} // namespace game 