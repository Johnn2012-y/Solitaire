#include "game/Foundation.hpp"

namespace game {

Foundation::Foundation(card::Suit s) : suit(s) {}

bool Foundation::canAdd(const card::Card& card) const {
    if (card.getSuit() != suit) return false;
    if (cards.empty()) return card.getRank() == card::Rank::Ace;
    return static_cast<int>(card.getRank()) == static_cast<int>(cards.back().getRank()) + 1;
}

card::Suit Foundation::getSuit() const {
    return suit;
}

bool Foundation::isComplete() const {
    return cards.size() == 13; // All cards from Ace to King
}

} // namespace game 