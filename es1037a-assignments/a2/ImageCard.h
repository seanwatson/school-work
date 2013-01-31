#pragma once
#include "VisualCard.h"


class ImageCard : public VisualCard
{
public:
    ImageCard(Suit suit = HEARTS, Denomination denomination = N2); // a constructor with default arguments
    ~ImageCard(void); // destructor
	
    // Draws the card at a given location inside a window where
    // 'pos' indicates the coordinates (in pixels) of the card's top-left corner
    //  w.r.t. the top-left corner of the window
	void drawAt(Point pos); // overwrites function with the same name in the base class (VisualCard)

private:	
	int m_im_face;   // stores face-side image handle (index) 
	int m_im_back;   // stores back-side image handle (index)
};
