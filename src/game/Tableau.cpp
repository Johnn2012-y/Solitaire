#include "game/Tableau.hpp"

namespace game {

Tableau::Tableau() {}

bool Tableau::canAdd(const card::Card& card) const {
    if (cards.empty()) return card.getRank() == card::Rank::King;
    return card.isRed() != cards.back().isRed() && 
           static_cast<int>(card.getRank()) == static_cast<int>(cards.back().getRank()) - 1;
}

bool Tableau::canAddSequence(const std::vector<card::Card>& sequence) const {
    if (sequence.empty()) return false;
    return canAdd(sequence.front());
}

void Tableau::addInitialCard(const card::Card& card, bool faceUp) {
    if (faceUp) {
        cards.push_back(card);
    } else {
        faceDown.push_back(card);
    }
}

void Tableau::turnOverCard() {
    if (!faceDown.empty()) {
        cards.push_back(faceDown.back());
        faceDown.pop_back();
    }
}

void Tableau::turnBackCard() {
    if (!cards.empty()) {
        faceDown.push_back(cards.back());
        cards.pop_back();
    }
}

const std::vector<card::Card>& Tableau::getFaceUp() const {
    return cards;
}

const std::vector<card::Card>& Tableau::getFaceDown() const {
    return faceDown;
}

bool Tableau::hasFaceDownCards() const {
    return !faceDown.empty();
}

size_t Tableau::getFaceDownCount() const {
    return faceDown.size();
}

} // namespace game 