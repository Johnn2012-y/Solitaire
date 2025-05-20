#pragma once

#include <string>
#include <wstring>

namespace card {

enum class Suit { Hearts, Diamonds, Clubs, Spades };
enum class Rank { Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King };

class Card {
public:
    Card();
    Card(Suit suit, Rank rank);
    
    Suit getSuit() const;
    Rank getRank() const;
    bool isRed() const;
    std::wstring toWString() const;
    std::string toString() const;
    
    bool operator==(const Card& other) const;
    bool operator!=(const Card& other) const;

private:
    Suit suit;
    Rank rank;
};

} // namespace card 