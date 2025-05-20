#include "game/GameManager.hpp"
#include "card/Deck.hpp"
#include <algorithm>
#include <stdexcept>

namespace game {

class DrawCommand : public Command {
public:
    DrawCommand(GameManager* game, int numToDraw)
        : game(game), numToDraw(numToDraw), oldWindowStart(0), oldWindowSize(0) {}
    
    void execute() override {
        oldWindowStart = game->wasteWindowStart;
        oldWindowSize = game->wasteWindowSize;
        
        if (game->stock.empty() && !game->waste.getCards().empty()) {
            std::vector<card::Card> wasteCards = game->waste.reset();
            std::reverse(wasteCards.begin(), wasteCards.end());
            game->stock.reset(wasteCards);
        }
        
        int toDraw = std::min(numToDraw, static_cast<int>(game->stock.size()));
        for (int i = 0; i < toDraw; ++i) {
            game->waste.addCard(game->stock.removeTop());
        }
        
        game->wasteWindowSize = toDraw;
        game->wasteWindowStart = game->waste.size() - game->wasteWindowSize;
    }
    
    void undo() override {
        for (size_t i = 0; i < game->wasteWindowSize; ++i) {
            game->stock.addCard(game->waste.removeTop());
        }
        game->wasteWindowStart = oldWindowStart;
        game->wasteWindowSize = oldWindowSize;
    }
    
private:
    GameManager* game;
    int numToDraw;
    size_t oldWindowStart;
    size_t oldWindowSize;
};

class MoveCardCommand : public Command {
public:
    MoveCardCommand(GameManager* game, const std::string& from, const std::string& to)
        : game(game), from(from), to(to), executed(false) {}
    
    void execute() override {
        if (!game->validateMove(from, to)) {
            return;
        }
        
        card::Card card;
        if (from == "W") {
            card = game->waste.getTop();
            game->waste.removeTop();
        } else {
            int fromIdx = game->parseIndex(from);
            card = game->tableaus[fromIdx].removeTop();
            if (game->tableaus[fromIdx].empty() && game->tableaus[fromIdx].hasFaceDownCards()) {
                game->tableaus[fromIdx].turnOverCard();
            }
        }
        
        if (to[0] == 'F') {
            int toIdx = game->parseIndex(to);
            game->foundations[toIdx].addCard(card);
        } else {
            int toIdx = game->parseIndex(to);
            game->tableaus[toIdx].addCard(card);
        }
        
        executed = true;
    }
    
    void undo() override {
        if (!executed) return;
        
        card::Card card;
        if (to[0] == 'F') {
            int toIdx = game->parseIndex(to);
            card = game->foundations[toIdx].removeTop();
        } else {
            int toIdx = game->parseIndex(to);
            card = game->tableaus[toIdx].removeTop();
        }
        
        if (from == "W") {
            game->waste.addCard(card);
        } else {
            int fromIdx = game->parseIndex(from);
            game->tableaus[fromIdx].addCard(card);
            if (game->tableaus[fromIdx].empty() && game->tableaus[fromIdx].hasFaceDownCards()) {
                game->tableaus[fromIdx].turnBackCard();
            }
        }
    }
    
    bool isExecuted() const override { return executed; }
    
private:
    GameManager* game;
    std::string from;
    std::string to;
    bool executed;
};

GameManager::GameManager(int numTab, int drawN)
    : moveCount(0), drawCount(drawN), numTableaus(numTab),
      wasteWindowStart(0), wasteWindowSize(0) {
    for (int s = static_cast<int>(card::Suit::Hearts); s <= static_cast<int>(card::Suit::Spades); ++s) {
        foundations.emplace_back(static_cast<card::Suit>(s));
    }
    tableaus.resize(numTableaus);
}

void GameManager::setup() {
    card::Deck deck;
    deck.shuffle();
    
    for (int i = 0; i < numTableaus; ++i) {
        for (int j = 0; j < i; ++j) {
            tableaus[i].addInitialCard(deck.draw(), false);
        }
        tableaus[i].addInitialCard(deck.draw(), true);
    }
    
    while (!deck.empty()) {
        stock.addCard(deck.draw());
    }
}

void GameManager::play() {
    setup();
    moveCount = 0;
    clearUndoStack();
}

bool GameManager::checkWin() const {
    return std::all_of(foundations.begin(), foundations.end(),
                      [](const Foundation& f) { return f.isComplete(); });
}

int GameManager::getMoveCount() const {
    return moveCount;
}

void GameManager::drawCards() {
    auto cmd = std::make_unique<DrawCommand>(this, drawCount);
    cmd->execute();
    undoStack.push_back(std::move(cmd));
    if (undoStack.size() > 3) {
        undoStack.erase(undoStack.begin());
    }
    ++moveCount;
}

bool GameManager::moveCard(const std::string& from, const std::string& to) {
    auto cmd = std::make_unique<MoveCardCommand>(this, from, to);
    cmd->execute();
    if (cmd->isExecuted()) {
        undoStack.push_back(std::move(cmd));
        if (undoStack.size() > 3) {
            undoStack.erase(undoStack.begin());
        }
        ++moveCount;
        return true;
    }
    return false;
}

bool GameManager::moveSequence(int fromIdx, int toIdx, size_t numCards) {
    if (fromIdx < 0 || fromIdx >= numTableaus || toIdx < 0 || toIdx >= numTableaus) {
        return false;
    }
    
    std::vector<card::Card> sequence = tableaus[fromIdx].removeSequence(numCards);
    if (tableaus[toIdx].canAddSequence(sequence)) {
        tableaus[toIdx].addCards(sequence);
        if (tableaus[fromIdx].empty() && tableaus[fromIdx].hasFaceDownCards()) {
            tableaus[fromIdx].turnOverCard();
        }
        ++moveCount;
        return true;
    }
    
    tableaus[fromIdx].addCards(sequence);
    return false;
}

void GameManager::undo() {
    if (!undoStack.empty()) {
        undoStack.back()->undo();
        undoStack.pop_back();
        --moveCount;
    }
}

void GameManager::restart() {
    clearUndoStack();
    moveCount = 0;
    stock = Stock();
    waste = Waste();
    foundations.clear();
    for (int s = static_cast<int>(card::Suit::Hearts); s <= static_cast<int>(card::Suit::Spades); ++s) {
        foundations.emplace_back(static_cast<card::Suit>(s));
    }
    tableaus.clear();
    tableaus.resize(numTableaus);
    setup();
}

const std::vector<Foundation>& GameManager::getFoundations() const {
    return foundations;
}

const std::vector<Tableau>& GameManager::getTableaus() const {
    return tableaus;
}

const Stock& GameManager::getStock() const {
    return stock;
}

const Waste& GameManager::getWaste() const {
    return waste;
}

size_t GameManager::getWasteWindowStart() const {
    return wasteWindowStart;
}

size_t GameManager::getWasteWindowSize() const {
    return wasteWindowSize;
}

void GameManager::clearUndoStack() {
    undoStack.clear();
}

bool GameManager::validateMove(const std::string& from, const std::string& to) const {
    if (from.empty() || to.empty()) return false;
    
    card::Card card;
    if (from == "W") {
        if (waste.empty()) return false;
        card = waste.getTop();
    } else if (from[0] == 'T') {
        int fromIdx = parseIndex(from);
        if (fromIdx < 0 || fromIdx >= numTableaus) return false;
        if (tableaus[fromIdx].empty()) return false;
        card = tableaus[fromIdx].getTop();
    } else {
        return false;
    }
    
    if (to[0] == 'F') {
        int toIdx = parseIndex(to);
        if (toIdx < 0 || toIdx >= 4) return false;
        return foundations[toIdx].canAdd(card);
    } else if (to[0] == 'T') {
        int toIdx = parseIndex(to);
        if (toIdx < 0 || toIdx >= numTableaus) return false;
        return tableaus[toIdx].canAdd(card);
    }
    
    return false;
}

int GameManager::parseIndex(const std::string& str) const {
    try {
        return std::stoi(str.substr(1)) - 1;
    } catch (const std::exception&) {
        return -1;
    }
}

} // namespace game 