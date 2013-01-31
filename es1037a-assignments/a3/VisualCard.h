#pragma once
#include "Card.h"
#include "Rect.h"

#define CARD_WIDTH 70 // default card width in pixels
#define CARD_HEIGHT 95 // default card height in pixels

class VisualCard : public Card
{
public:
    VisualCard(Suit suit = HEARTS, Denomination denomination = N2); // a constructor with default arguments
    ~VisualCard(void); // destructor

    inline bool isFaceUp(void) const {return m_face_up;}
    inline void setFaceUp(void) {m_face_up = true;}
    inline void setFaceDown(void) {m_face_up = false;}

    // Checks if given 'point' is inside the rectangular area that
    // the card currently occupies on the screen
    inline bool pointInCard(Point point) const {return m_rect.ptInRect(point);}
	
    // Draws the card at a given location inside a window where
    // 'pos' indicates the coordinates (in pixels) of the card's top-left corner
    //  w.r.t. the top-left corner of the window
	void drawAt(Point pos);

private:
	bool m_face_up; 

protected:	// 'protected' member variables are accessible only for derived objects (e.g. for 'ImageCards')
	Rect m_rect;   //rectangular area that the card occupies on the screen;
};
