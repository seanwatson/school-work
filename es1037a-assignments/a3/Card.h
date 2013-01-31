#pragma once

// User defined Suit and Denomination types (user-defined integers) 
enum Suit {SPADES=1, HEARTS=2, CLUBS=3, DIAMONDS=4};
enum Denomination {N2=2, N3=3, N4=4, N5=5, N6=6, N7=7, N8=8, N9=9, N10=10, JACK=11, QUEEN=12, KING=13, ACE=14};



class Card
{
public:
    Card(Suit suit = HEARTS, Denomination denomination = N2); // constructor with some default values
    ~Card(void); // destructor
    inline Suit getSuit(void) const {return m_suit;}
    inline Denomination getDenomination(void) const {return m_denomination;}
    char toCharSuit(void); // returns a char that "describes" card's suit
    char toCharDenomination(void); // returns a char that "describes" card's denomination

private:
    const Suit m_suit; // private storage of suit
    const Denomination m_denomination; // private storage of denomination
};

// global operator functions comparing denominations of two cards
// The cards a and b are passed by reference
inline bool operator<(const Card& a, const Card& b) {
    if ( a.getSuit()==b.getSuit() ) return ( a.getDenomination()<b.getDenomination() );
    else                            return ( a.getSuit()<b.getSuit() ); 
}
inline bool operator>(const Card& a, const Card& b)  {
    if ( a.getSuit()==b.getSuit() ) return ( a.getDenomination()>b.getDenomination() );
    else                            return ( a.getSuit()>b.getSuit() ); 
}

// checks equality of suits and denominations for two cards
inline bool operator==(Card& a, Card& b)  {return (a.getDenomination()==b.getDenomination() && a.getSuit()==b.getSuit());}
