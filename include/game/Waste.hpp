#pragma once

#include "game/Pile.hpp"

namespace game {

class Waste : public Pile {
public:
    Waste();
    
    bool canAdd(const card::Card& card) const override;
    std::vector<card::Card> reset();
};

} // namespace game 