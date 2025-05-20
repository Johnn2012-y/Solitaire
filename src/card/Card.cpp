#include "card/Card.hpp"
#include <sstream>

namespace card {

Card::Card() : suit(Suit::Hearts), rank(Rank::Ace) {}

Card::Card(Suit s, Rank r) : suit(s), rank(r) {}

Suit Card::getSuit() const {
    return suit;
}

Rank Card::getRank() const {
    return rank;
}

bool Card::isRed() const {
    return suit == Suit::Hearts || suit == Suit::Diamonds;
}

std::wstring Card::toWString() const {
    static const std::wstring ranks = L"A23456789TJQK";
    static const std::wstring suits = L"\u2665\u2666\u2663\u2660"; // ♥♦♣♠
    return std::wstring(1, ranks[static_cast<int>(rank)]) + suits[static_cast<int>(suit)];
}

std::string Card::toString() const {
    static const std::string ranks = "A23456789TJQK";
    static const std::string suits = "HDCS"; // Hearts, Diamonds, Clubs, Spades
    return std::string(1, ranks[static_cast<int>(rank)]) + suits[static_cast<int>(suit)];
}

bool Card::operator==(const Card& other) const {
    return suit == other.suit && rank == other.rank;
}

bool Card::operator!=(const Card& other) const {
    return !(*this == other);
}

} // namespace card 