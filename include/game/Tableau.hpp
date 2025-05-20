#pragma once

#include "game/Pile.hpp"
#include <vector>

namespace game {

class Tableau : public Pile {
public:
    Tableau();
    
    bool canAdd(const card::Card& card) const override;
    bool canAddSequence(const std::vector<card::Card>& sequence) const;
    void addInitialCard(const card::Card& card, bool faceUp);
    void turnOverCard();
    void turnBackCard();
    
    const std::vector<card::Card>& getFaceUp() const;
    const std::vector<card::Card>& getFaceDown() const;
    bool hasFaceDownCards() const;
    size_t getFaceDownCount() const;

private:
    std::vector<card::Card> faceDown;
};

} // namespace game 