#pragma once

#include "card/Card.hpp"
#include <vector>

namespace game {

class Pile {
public:
    virtual ~Pile() = default;
    
    virtual bool canAdd(const card::Card& card) const = 0;
    virtual void addCard(const card::Card& card);
    virtual void addCards(const std::vector<card::Card>& cards);
    virtual card::Card removeTop();
    virtual std::vector<card::Card> removeSequence(size_t num);
    virtual const card::Card& getTop() const;
    virtual bool empty() const;
    virtual size_t size() const;
    virtual const std::vector<card::Card>& getCards() const;

protected:
    std::vector<card::Card> cards;
};

} // namespace game 