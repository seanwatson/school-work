#include "ImageCard.h"
#include "cs037util.h"
#include <iostream>   // for sprintf_s()


// constructor
ImageCard::ImageCard(Suit suit, Denomination denomination)
: VisualCard(suit,denomination) // calls a constructor of the base class
{
	char face_image_file_name[50], back_image_file_name[50];
    sprintf_s(face_image_file_name,50,"./cards_bmp/%c%c.bmp",toCharSuit(),toCharDenomination());
	sprintf_s(back_image_file_name,50,"./cards_bmp/b1fv.bmp");
	m_im_face = CreateImageFromFile(face_image_file_name); // loading 'face' image
	m_im_back = CreateImageFromFile(back_image_file_name); // loading 'back' image
}

// destructor
ImageCard::~ImageCard(void)
{
	// deallocating images from RAM
	if (m_im_face!=0) DeleteImage(m_im_face);
	if (m_im_back!=0) DeleteImage(m_im_back);
}

// Draws the card at a given location inside a window where
// 'pos' indicates the coordinates (in pixels) of the card's top-left corner
//  w.r.t. the top-left corner of the window
void ImageCard::drawAt(Point pos)
{	
	// in case images were not found, calls the base class 'drawAt' function
	if (m_im_face==0 || m_im_back==0) {VisualCard::drawAt(pos); return;} 

	if (isFaceUp()) {// drawing the card in the FACE-UP state
		m_rect=Rect(pos,GetImageSizeX(m_im_face),GetImageSizeY(m_im_face)); // area where the card appears on the monitor
		DrawImage(pos.x, pos.y, m_im_face); // function from cs037util
	}
	else {// drawing the card in the FACE-DOWN state 
		m_rect=Rect(pos,GetImageSizeX(m_im_back),GetImageSizeY(m_im_back)); // area where the card appears on the monitor
		DrawImage(pos.x, pos.y, m_im_back); // function from cs037util
	}
}
