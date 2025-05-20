#pragma once

#include "game/Pile.hpp"

namespace game {

class Stock : public Pile {
public:
    Stock();
    
    bool canAdd(const card::Card& card) const override;
    void reset(const std::vector<card::Card>& cards);
};

} // namespace game 