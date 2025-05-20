#include "card/Deck.hpp"
#include <chrono>

namespace card {

Deck::Deck() {
    // Initialize random number generator with current time
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
    
    // Create a standard deck of 52 cards
    for (int s = static_cast<int>(Suit::Hearts); s <= static_cast<int>(Suit::Spades); ++s) {
        for (int r = static_cast<int>(Rank::Ace); r <= static_cast<int>(Rank::King); ++r) {
            cards.emplace_back(static_cast<Suit>(s), static_cast<Rank>(r));
        }
    }
}

void Deck::shuffle() {
    std::shuffle(cards.begin(), cards.end(), rng);
}

Card Deck::draw() {
    if (cards.empty()) {
        return Card(Suit::Hearts, Rank::Ace); // Return a placeholder card
    }
    Card card = cards.back();
    cards.pop_back();
    return card;
}

bool Deck::empty() const {
    return cards.empty();
}

size_t Deck::size() const {
    return cards.size();
}

void Deck::addCard(const Card& card) {
    cards.push_back(card);
}

void Deck::addCards(const std::vector<Card>& newCards) {
    cards.insert(cards.end(), newCards.begin(), newCards.end());
}

const std::vector<Card>& Deck::getCards() const {
    return cards;
}

} // namespace card 