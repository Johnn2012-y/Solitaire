#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include "card.h"

class Game;  // Forward declaration

class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual bool isExecuted() const { return true; }
};

class DrawCommand : public Command {
private:
    Game* game;
    int numToDraw;
    std::vector<Card> drawnCards;
    size_t oldWindowStart;
    size_t oldWindowSize;
    bool wasStockEmpty;
    std::vector<Card> oldWasteCards;
    std::vector<Card> oldStockCards;

public:
    DrawCommand(Game* g, int n);
    void execute() override;
    void undo() override;
};

class MoveCardCommand : public Command {
private:
    Game* game;
    std::string from, to;
    Card movedCard;
    bool turnedCard;
    bool executed = false;
public:
    MoveCardCommand(Game* g, const std::string& f, const std::string& t);
    void execute() override;
    void undo() override;
    bool isExecuted() const { return executed; }
};

class MoveSequenceCommand : public Command {
private:
    Game* game;
    int fromIdx, toIdx;
    size_t numCards;
    std::vector<Card> movedSeq;
    bool turnedCard;
    bool executed = false;
public:
    MoveSequenceCommand(Game* g, int f, int t, size_t n);
    void execute() override;
    void undo() override;
    bool isExecuted() const { return executed; }
};

#endif // COMMANDS_H 