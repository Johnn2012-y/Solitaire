#ifndef GAME_COMPONENTS_H
#define GAME_COMPONENTS_H

#include "card.h"
#include <vector>

class Stock {
private:
    std::vector<Card> cards;
public:
    Card draw() {
        if (cards.empty()) return Card(Hearts, Ace);
        Card c = cards.back();
        cards.pop_back();
        return c;
    }
    bool isEmpty() const { return cards.empty(); }
    size_t size() const { return cards.size(); }
    void addCards(const std::vector<Card>& newCards) {
        cards.insert(cards.end(), newCards.begin(), newCards.end());
    }
};

class Waste {
private:
    std::vector<Card> cards;
public:
    Card getTop() const {
        if (cards.empty()) return Card(Hearts, Ace);
        return cards.back();
    }
    Card removeTop() {
        if (cards.empty()) return Card(Hearts, Ace);
        Card c = cards.back();
        cards.pop_back();
        return c;
    }
    void addCards(const std::vector<Card>& newCards) {
        cards.insert(cards.end(), newCards.begin(), newCards.end());
    }
    const std::vector<Card>& getCards() const { return cards; }
};

class Foundation {
private:
    Suit suit;
    std::vector<Card> cards;
public:
    Foundation(Suit s) : suit(s) {}
    bool canAdd(const Card& card) const {
        if (card.getSuit() != suit) return false;
        if (cards.empty()) return card.getRank() == Ace;
        return card.getRank() == static_cast<Rank>(cards.back().getRank() + 1);
    }
    void addCard(const Card& card) { cards.push_back(card); }
    Card getTop() const {
        if (cards.empty()) return Card(Hearts, Ace);
        return cards.back();
    }
    void removeTopCard() {
        if (!cards.empty()) cards.pop_back();
    }
    const std::vector<Card>& getCards() const { return cards; }
};

class Tableau {
private:
    std::vector<Card> faceDown;
    std::vector<Card> faceUp;
public:
    bool canAdd(const Card& card) const {
        if (faceUp.empty()) return card.getRank() == King;
        return card.isRed() != faceUp.back().isRed() && card.getRank() == static_cast<Rank>(faceUp.back().getRank() - 1);
    }
    bool canAddSequence(const std::vector<Card>& seq) const {
        if (seq.empty()) return false;
        return canAdd(seq[0]);
    }
    void addSequence(const std::vector<Card>& seq) {
        faceUp.insert(faceUp.end(), seq.begin(), seq.end());
    }
    std::vector<Card> removeSequence(size_t num) {
        if (num > faceUp.size()) num = faceUp.size();
        std::vector<Card> seq(faceUp.end() - num, faceUp.end());
        faceUp.erase(faceUp.end() - num, faceUp.end());
        if (faceUp.empty() && !faceDown.empty()) turnOverCard();
        return seq;
    }
    void turnOverCard() {
        if (!faceDown.empty()) {
            faceUp.push_back(faceDown.back());
            faceDown.pop_back();
        }
    }
    void addInitialCard(const Card& card, bool faceUpFlag) {
        if (faceUpFlag) faceUp.push_back(card);
        else faceDown.push_back(card);
    }
    void removeLastCards(size_t num) {
        if (num > faceUp.size()) num = faceUp.size();
        faceUp.erase(faceUp.end() - num, faceUp.end());
    }
    void turnBackCard() {
        if (!faceUp.empty()) {
            Card c = faceUp.front();
            faceUp.erase(faceUp.begin());
            faceDown.push_back(c);
        }
    }
    const std::vector<Card>& getFaceUp() const { return faceUp; }
    const std::vector<Card>& getFaceDown() const { return faceDown; }
};

#endif // GAME_COMPONENTS_H 