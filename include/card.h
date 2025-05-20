#ifndef CARD_H
#define CARD_H

#include <vector>
#include <string>
#include <random>
#include <algorithm>

enum Suit { Hearts, Diamonds, Clubs, Spades };
enum Rank { Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King };

class Card {
private:
    Suit suit;
    Rank rank;
public:
    Card() : suit(Hearts), rank(Ace) {}
    Card(Suit s, Rank r) : suit(s), rank(r) {}
    Suit getSuit() const { return suit; }
    Rank getRank() const { return rank; }
    std::wstring toWString() const {
        static const std::wstring ranks = L"A23456789TJQK";
        static const std::wstring suits = L"\u2665\u2666\u2663\u2660"; // ♥♦♣♠
        return std::wstring(1, ranks[rank]) + suits[suit];
    }
    bool isRed() const { return suit == Hearts || suit == Diamonds; }
};

class Deck {
private:
    std::vector<Card> cards;
public:
    Deck() {
        for (int s = Hearts; s <= Spades; ++s)
            for (int r = Ace; r <= King; ++r)
                cards.emplace_back(static_cast<Suit>(s), static_cast<Rank>(r));
    }
    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }
    Card draw() {
        if (cards.empty()) return Card(Hearts, Ace);
        Card c = cards.back();
        cards.pop_back();
        return c;
    }
    bool empty() const { return cards.empty(); }
};

#endif // CARD_H 