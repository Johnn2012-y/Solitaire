#pragma once

#include "game/Pile.hpp"

namespace game {

class Foundation : public Pile {
public:
    explicit Foundation(card::Suit suit);
    
    bool canAdd(const card::Card& card) const override;
    card::Suit getSuit() const;
    bool isComplete() const;

private:
    card::Suit suit;
};

} // namespace game 