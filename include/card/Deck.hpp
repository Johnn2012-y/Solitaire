#pragma once

#include "card/Card.hpp"
#include <vector>
#include <random>

namespace card {

class Deck {
public:
    Deck();
    
    void shuffle();
    Card draw();
    bool empty() const;
    size_t size() const;
    void addCard(const Card& card);
    void addCards(const std::vector<Card>& cards);
    const std::vector<Card>& getCards() const;

private:
    std::vector<Card> cards;
    std::mt19937 rng;
};

} // namespace card 