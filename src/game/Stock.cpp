#include "game/Stock.hpp"

namespace game {

Stock::Stock() {}

bool Stock::canAdd(const card::Card& card) const {
    return true; // Stock can accept any card
}

void Stock::reset(const std::vector<card::Card>& newCards) {
    cards = newCards;
}

} // namespace game 