#include "../include/commands.h"
#include "../include/game.h"
#include <iostream>

DrawCommand::DrawCommand(Game* g, int n) : game(g), numToDraw(n) {}

void DrawCommand::execute() {
    oldWindowStart = game->wasteWindowStart;
    oldWindowSize = game->wasteWindowSize;
    drawnCards.clear();
    wasStockEmpty = game->stock.isEmpty();
    oldWasteCards = game->waste.getCards();

    if (game->stock.isEmpty() && !game->waste.getCards().empty()) {
        std::vector<Card> wasteCards = game->waste.getCards();
        game->waste = Waste();
        std::reverse(wasteCards.begin(), wasteCards.end());
        game->stock.addCards(wasteCards);
    }

    int toDraw = std::min(numToDraw, static_cast<int>(game->stock.size()));
    for (int i = 0; i < toDraw; ++i) {
        Card c = game->stock.draw();
        drawnCards.push_back(c);
        game->waste.addCards({c});
    }
    
    game->wasteWindowSize = drawnCards.size();
    game->wasteWindowStart = game->waste.getCards().size() - game->wasteWindowSize;
}

void DrawCommand::undo() {
    // Usuwamy dobrane karty z waste
    for (const auto& card : drawnCards) {
        game->waste.removeTop();
    }

    // Przywracamy oryginalny stan
    if (wasStockEmpty) {
        // Jeśli stock był pusty, to znaczy że karty zostały dobrane z waste
        // więc musimy je z powrotem dodać do waste
        game->waste = Waste();
        game->waste.addCards(oldWasteCards);
    } else {
        // Jeśli stock nie był pusty, to karty zostały dobrane ze stock
        // więc musimy je z powrotem dodać do stock
        for (auto it = drawnCards.rbegin(); it != drawnCards.rend(); ++it) {
            game->stock.addCards({*it});
        }
        // Przywracamy oryginalny stan waste
        game->waste = Waste();
        game->waste.addCards(oldWasteCards);
    }

    // Przywracamy oryginalny rozmiar i pozycję okna waste
    game->wasteWindowSize = oldWindowSize;
    game->wasteWindowStart = oldWindowStart;
}

MoveCardCommand::MoveCardCommand(Game* g, const std::string& f, const std::string& t) 
    : game(g), from(f), to(t), turnedCard(false) {}

void MoveCardCommand::execute() {
    Card card;
    bool fromWaste = false;
    bool fromFoundation = false;
    int fromIdx = -1;

    if (from == "W") {
        if (game->waste.getCards().empty()) return;
        card = game->waste.getTop();
        movedCard = card;
        fromWaste = true;
    }
    else if (from[0] == 'F') {
        fromIdx = std::stoi(from.substr(1)) - 1;
        if (fromIdx < 0 || fromIdx >= 4 || game->foundations[fromIdx].getCards().empty()) return;
        card = game->foundations[fromIdx].getTop();
        movedCard = card;
        fromFoundation = true;
    }
    else if (from[0] == 'T') {
        fromIdx = std::stoi(from.substr(1)) - 1;
        if (fromIdx < 0 || fromIdx >= game->numTableaus) return;
        const auto& up = game->tableaus[fromIdx].getFaceUp();
        if (up.empty()) return;
        card = up.back();
        turnedCard = (up.size() == 1 && !game->tableaus[fromIdx].getFaceDown().empty());
    }
    else return;

    if (to[0] == 'F') {
        int fIdx = std::stoi(to.substr(1)) - 1;
        if (fIdx < 0 || fIdx >= 4 || !game->foundations[fIdx].canAdd(card)) {
            std::cout << "Nie można położyć na " << to << "!\n";
            return;
        }
    }
    else if (to[0] == 'T') {
        int tIdx = std::stoi(to.substr(1)) - 1;
        if (tIdx < 0 || tIdx >= game->numTableaus || !game->tableaus[tIdx].canAdd(card)) {
            std::cout << "Nie można położyć na " << to << "!\n";
            return;
        }
    }
    else return;

    if (fromWaste) {
        game->waste.removeTop();
    } else if (fromFoundation) {
        game->foundations[fromIdx].removeTopCard();
    } else {
        auto seq = game->tableaus[fromIdx].removeSequence(1);
        movedCard = seq[0];
    }

    if (to[0] == 'F') {
        int fIdx = std::stoi(to.substr(1)) - 1;
        game->foundations[fIdx].addCard(card);
    } else {
        int tIdx = std::stoi(to.substr(1)) - 1;
        game->tableaus[tIdx].addSequence({card});
    }
    executed = true;
}

void MoveCardCommand::undo() {
    if (to[0] == 'F') {
        int idx = std::stoi(to.substr(1)) - 1;
        if (idx >= 0 && idx < 4) game->foundations[idx].removeTopCard();
    } else if (to[0] == 'T') {
        int idx = std::stoi(to.substr(1)) - 1;
        if (idx >= 0 && idx < game->numTableaus) game->tableaus[idx].removeLastCards(1);
    }

    if (from == "W") {
        game->waste.addCards({movedCard});
    } else if (from[0] == 'F') {
        int idx = std::stoi(from.substr(1)) - 1;
        if (idx >= 0 && idx < 4) {
            game->foundations[idx].addCard(movedCard);
        }
    } else if (from[0] == 'T') {
        int idx = std::stoi(from.substr(1)) - 1;
        if (idx >= 0 && idx < game->numTableaus) {
            game->tableaus[idx].addSequence({movedCard});
            if (turnedCard) {
                game->tableaus[idx].turnBackCard();
            }
        }
    }
}

MoveSequenceCommand::MoveSequenceCommand(Game* g, int f, int t, size_t n) 
    : game(g), fromIdx(f), toIdx(t), numCards(n), turnedCard(false), executed(false) {}

void MoveSequenceCommand::execute() {
    if (fromIdx >= 0 && fromIdx < game->numTableaus && toIdx >= 0 && toIdx < game->numTableaus) {
        const auto& faceUp = game->tableaus[fromIdx].getFaceUp();
        const auto& faceDown = game->tableaus[fromIdx].getFaceDown();
        
        if (numCards > faceUp.size()) {
            std::cout << "Cannot move face-down cards!\n";
            return;
        }
        
        movedSeq = game->tableaus[fromIdx].removeSequence(numCards);
        turnedCard = game->tableaus[fromIdx].getFaceUp().empty() && !game->tableaus[fromIdx].getFaceDown().empty();
        if (game->tableaus[toIdx].canAddSequence(movedSeq)) {
            game->tableaus[toIdx].addSequence(movedSeq);
            executed = true;
        } else {
            game->tableaus[fromIdx].addSequence(movedSeq);
            if (turnedCard) {
                game->tableaus[fromIdx].turnBackCard();
            }
        }
    }
}

void MoveSequenceCommand::undo() {
    if (fromIdx >= 0 && fromIdx < game->numTableaus && toIdx >= 0 && toIdx < game->numTableaus) {
        game->tableaus[toIdx].removeLastCards(numCards);
        game->tableaus[fromIdx].addSequence(movedSeq);
        if (turnedCard) {
            game->tableaus[fromIdx].turnBackCard();
        }
    }
} 