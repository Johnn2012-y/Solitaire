#include "game/Waste.hpp"

namespace game {

Waste::Waste() {}

bool Waste::canAdd(const card::Card& card) const {
    return true; // Waste can accept any card
}

std::vector<card::Card> Waste::reset() {
    std::vector<card::Card> allCards = cards;
    cards.clear();
    return allCards;
}

} // namespace game 