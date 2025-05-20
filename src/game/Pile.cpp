#include "game/Pile.hpp"

namespace game {

void Pile::addCard(const card::Card& card) {
    cards.push_back(card);
}

void Pile::addCards(const std::vector<card::Card>& newCards) {
    cards.insert(cards.end(), newCards.begin(), newCards.end());
}

card::Card Pile::removeTop() {
    if (cards.empty()) {
        return card::Card(card::Suit::Hearts, card::Rank::Ace); // Placeholder
    }
    card::Card card = cards.back();
    cards.pop_back();
    return card;
}

std::vector<card::Card> Pile::removeSequence(size_t num) {
    if (num > cards.size()) {
        num = cards.size();
    }
    std::vector<card::Card> sequence(cards.end() - num, cards.end());
    cards.erase(cards.end() - num, cards.end());
    return sequence;
}

const card::Card& Pile::getTop() const {
    static const card::Card placeholder(card::Suit::Hearts, card::Rank::Ace);
    return cards.empty() ? placeholder : cards.back();
}

bool Pile::empty() const {
    return cards.empty();
}

size_t Pile::size() const {
    return cards.size();
}

const std::vector<card::Card>& Pile::getCards() const {
    return cards;
}

} // namespace game 