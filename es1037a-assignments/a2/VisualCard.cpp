#include "VisualCard.h"
#include "cs037util.h"

// constructor
VisualCard::VisualCard(Suit suit, Denomination denomination)
: m_face_up(false)
, m_rect()
, Card(suit,denomination) // calls a constructor of the base class
{
}

// destructor
VisualCard::~VisualCard(void)
{
}

// Draws the card at a given location inside a window where
// 'pos' indicates the coordinates (in pixels) of the card's top-left corner
//  w.r.t. the top-left corner of the window
void VisualCard::drawAt(Point pos)
{	
	m_rect=Rect(pos,CARD_WIDTH,CARD_HEIGHT); // area where the card will appears on the monitor
    if (isFaceUp()) // drawing the card in the FACE-UP state
    {
        // coloring card's face side
        SetDrawColour(0.7,0.7,0.7);
        DrawRectangleFilled(m_rect.get_left(),m_rect.get_top(),m_rect.get_right(),m_rect.get_bottom());

        char text[2]="0";
        SetDrawColour(0.0,0.0,0.0);
        text[0]=toCharSuit();
        DrawString(m_rect.get_left()+10,m_rect.get_top()+8,text);
        text[0] = toCharDenomination();
        DrawString(m_rect.get_left()+10,m_rect.get_top()+30,text);
	}
    else // drawing the card in the FACE-DOWN state
    {
        // coloring card's back
        SetDrawColour(0.0,0.8,0.0);
        DrawRectangleFilled(m_rect.get_left(),m_rect.get_top(),m_rect.get_right(),m_rect.get_bottom());
    }
}
