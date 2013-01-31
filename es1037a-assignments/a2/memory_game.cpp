#include "cs037util.h"
#include <iostream>
#include <time.h>
#include "Bag.h"
#include "ImageCard.h"

using namespace std;

// global variables
ImageCard ** cards = NULL; //(dynamic) array of pointers to VisualCard(s) in the layout
unsigned size = 0; // size of array 'cards'
// YOU CAN DECLARE NECESSARY GLOBAL VARIABLES HERE (DO NOT USE MORE VARIABLES THAN WHAT IS ABSOLUTELY NECESSARY)
//....
Bag<ImageCard *> deck(52);
ImageCard* first(NULL);
ImageCard* second(NULL);
int matches(0), not_matches(0);


//global functions/methods: DO NOT DECLARE ANY OTHER GLOBAL FUNCTIONS
void clear_cards();
void init_cards();
void draw_cards();
void reveal_card(unsigned i); 
void check_pair_matches(); 

// the "main" function: DO NOT CHANGE CODE FOR "MAIN" ITSELF !!!!!!!!!!!!!
int main()
{
	cout << " 'q' - to quit\n 'n' - to start a new game" << endl << endl;
	srand( (unsigned) time( NULL ) ); // initialize random number generator
	init_cards();
	draw_cards();
	ShowWindow("Memory Game");

	bool quit_requested = false;
	while (quit_requested == false)
	{
		// READING KEYBOARD INPUT, IF ANY
		char c;
		if (GetKeyboardInput(&c)) // check keyboard
		{
			if (c == 'q') quit_requested = true; // if asked to quit, break from loop
			else if (c == 'n')             // user pressed 'n' for a new game
			{
				clear_cards();
				init_cards();
				draw_cards();
			}
		}

		// READING MOUSE INPUT, IF ANY
		int x, y, button;
		if (GetMouseInput(&x, &y, &button)) // check mouse 
		{
			if (button == 1)                // button 1 means the left button
			{	
				for (unsigned i=0; i<size; i++) 
					if (cards[i] && cards[i]->pointInCard(Point(x,y))) reveal_card(i);
				draw_cards();
			}
		}
	}
	clear_cards();
	return 0;
}

void draw_cards()
{
	// Clear the window to white
	SetDrawColour(1.0,1.0,1.0); DrawRectangleFilled(0,0,1280,1024);
	SetDrawColour(1.0,0.0,0.0); DrawString(10, 10, "Click on cards, choose pairs of the same denomination");

	// WRITE YOUR CODE FOR DISPLAYING "CARDS" (STORED VIA DYNAMIC ARRAY OF POINTERS)
	//...
	int remaining = size;                        //Draw the cards from left to right
	for (int i=0; remaining !=0; i++)            //Move one row down until there are no cards left
		for (int j=0; j<6 && remaining!=0; j++){ //Draw 6 cards in a row or until there are no cards
			if (cards[remaining-1]!=NULL){
				cards[remaining-1]->drawAt(Point((CARD_WIDTH+10)*(1+j),(CARD_HEIGHT+10)*(i+1)));
				}
			remaining--;                        //Subtract one from the cards left to draw
		}
}

void init_cards()
{
	size = 8 + 2*(rand()%7);  // an even random number between 8 and 20
	first = NULL;             //Reset first and second to null
	second = NULL;
	cards = new ImageCard*[size];           //Cards points to array of imagecard pointers
	deck.add( new ImageCard(HEARTS,ACE) );  //Rediculously inefficient way to create a deck
	deck.add( new ImageCard(HEARTS,KING) );
	deck.add( new ImageCard(HEARTS,QUEEN) );
	deck.add( new ImageCard(HEARTS,JACK) );
	deck.add( new ImageCard(HEARTS,N10) );
	deck.add( new ImageCard(HEARTS,N9) );
	deck.add( new ImageCard(HEARTS,N8) );
	deck.add( new ImageCard(HEARTS,N7) );
	deck.add( new ImageCard(HEARTS,N6) );
	deck.add( new ImageCard(HEARTS,N5) );
	deck.add( new ImageCard(HEARTS,N4) );
	deck.add( new ImageCard(HEARTS,N3) );
	deck.add( new ImageCard(HEARTS,N2) );
	deck.add( new ImageCard(DIAMONDS,ACE) );
	deck.add( new ImageCard(DIAMONDS,KING) );
	deck.add( new ImageCard(DIAMONDS,QUEEN) );
	deck.add( new ImageCard(DIAMONDS,JACK) );
	deck.add( new ImageCard(DIAMONDS,N10) );
	deck.add( new ImageCard(DIAMONDS,N9) );
	deck.add( new ImageCard(DIAMONDS,N8) );
	deck.add( new ImageCard(DIAMONDS,N7) );
	deck.add( new ImageCard(DIAMONDS,N6) );
	deck.add( new ImageCard(DIAMONDS,N5) );
	deck.add( new ImageCard(DIAMONDS,N4) );
	deck.add( new ImageCard(DIAMONDS,N3) );
	deck.add( new ImageCard(DIAMONDS,N2) );
	deck.add( new ImageCard(SPADES,ACE) );
	deck.add( new ImageCard(SPADES,KING) );
	deck.add( new ImageCard(SPADES,QUEEN) );
	deck.add( new ImageCard(SPADES,JACK) );
	deck.add( new ImageCard(SPADES,N10) );
	deck.add( new ImageCard(SPADES,N9) );
	deck.add( new ImageCard(SPADES,N8) );
	deck.add( new ImageCard(SPADES,N7) );
	deck.add( new ImageCard(SPADES,N6) );
	deck.add( new ImageCard(SPADES,N5) );
	deck.add( new ImageCard(SPADES,N4) );
	deck.add( new ImageCard(SPADES,N3) );
	deck.add( new ImageCard(SPADES,N2) );
	deck.add( new ImageCard(CLUBS,ACE) );
	deck.add( new ImageCard(CLUBS,KING) );
	deck.add( new ImageCard(CLUBS,QUEEN) );
	deck.add( new ImageCard(CLUBS,JACK) );
	deck.add( new ImageCard(CLUBS,N10) );
	deck.add( new ImageCard(CLUBS,N9) );
	deck.add( new ImageCard(CLUBS,N8) );
	deck.add( new ImageCard(CLUBS,N7) );
	deck.add( new ImageCard(CLUBS,N6) );
	deck.add( new ImageCard(CLUBS,N5) );
	deck.add( new ImageCard(CLUBS,N4) );
	deck.add( new ImageCard(CLUBS,N3) );
	deck.add( new ImageCard(CLUBS,N2) );

	for (unsigned i=0; i<size; i=i+2) {
		cards[i]=deck.getOne();
		cards[i+1]=deck.getOne();            //Get two cards
		while (cards[i]->getDenomination()!=cards[i+1]->getDenomination()){ //If they arent the same, put the 2nd card back and get a new one
			deck.add(new ImageCard(cards[i+1]->getSuit(),cards[i+1]->getDenomination()));
			delete cards[i+1];
			cards[i+1]=deck.getOne();
		}
		}
		while (!deck.isEmpty()){ //Delete the rest of the cards in the deck
		first=deck.getOne();
		delete first;
		first = NULL;
		}
		for (unsigned i=0; i<size; i++){ //Put the cards back in the bag
			deck.add(new ImageCard(cards[i]->getSuit(),cards[i]->getDenomination()));
			delete cards[i];
			cards[i]=NULL;
		}
		for (unsigned i=0; i<size; i++) //Draw them again to randomize the order
			cards[i]=deck.getOne();
	
	// ALLOCATE DYNAMIC ARRAY ("cards") OF GIVEN SIZE (size) AND FILL IT UP WITH POINTERS 
	// TO DYNAMICALLY CREATED ImageCards. THESE CARDS SHOULD BE RANDOMLY DRAWN FROM A DECK 
	// OF 52 CARDS IN SUCH A WAY THAT ANY CARD HAS A MATCHING CARD 
	// OF THE SAME DENOMINATION (BUT DIFFERENT SUIT). 
	// DECK OF CARDS MUST BE DYNAMICALLY ALLOCATED "BAG" FOR POINTERS TO ImageCards".
	//...

	// Print instructions on your console
	cout << endl << "WELCOME TO A NEW GAME!" << endl << endl;
	cout << "Select your first card" << endl;
}

void reveal_card(unsigned i)
{
	// the clicked card should be turned face up unless there are two "face-up" cards already.
	// print useful instructions on the console
	//...
	if (cards[i]->isFaceUp()!=true){ //Make sure the card isnt already face up
		if (first==NULL){ //If its the first card selected asign it to first and put it face up
			first = cards[i];
			first->setFaceUp();
		}
		else {
			second = cards[i]; //If its the second card selected assign it to second and put it face up
			second->setFaceUp();
		}
		draw_cards(); //Draw the cards now face up
		if (first && second) { //Check if there are two cards face up
			Pause(2000); 
			check_pair_matches(); //Check if they are the same
		}
		else
			cout << "Draw your second card." <<endl;
	}
	

	// In case a second "face-up" card was just selected, call "check_pair_matches()"
	// after a small delay allowing the user to see both cards face-up.
	// HINT: use function Pause(...) from 'cs037utils' to simulate a delay. 
}

void check_pair_matches()
{

	if (first->getDenomination()==second->getDenomination()){ //Check if the denominations are the same

		for (unsigned i=0; i<size; i++){ //If they are the same, find which cards they were and make those cards null
			if (first==cards[i])
				cards[i]=NULL;
			if (second==cards[i])
				cards[i]=NULL;
		}
		delete first; //Delete both cards
		delete second;
		cout << "Super! You found a match." << endl;
		matches++;

	}
	else {
		first->setFaceDown(); //If they arent a match set them both face down
		second->setFaceDown();
		cout << "Sorry, those aren't a match. Try again." << endl;
		not_matches++;
	}
	first=NULL;
	second=NULL;

	// In case there are two "selected" cards
	// the MemoryGame should proceed as follows: if both "face-up" cards
	// have the same denomination then you score one good pair and the corresponding cards
	// should be removed from array 'cards' and deleted (use NULL pointers to replace them). 
	// Otherwise, you score one bad pair and both cards should be turned face-down.  
	// Print useful instructions or information on the console
	//...
	
	// Check if the the game is over
	// Print useful information on the console
	//...
	if (matches==(size/2)){ //Check if game is over and display score
		cout << "\nGAME OVER. Correct matches = " << matches << ", Incorrect matches = " << not_matches << endl;

	}
	else
		cout << endl << "Select your first card." << endl;
	
}

void clear_cards()
{
		matches = 0; //Reset the score counters
		not_matches = 0;
	// delete all dynamically created objects
	// ...
	for (unsigned i=0; i<size; i++) //Delete all the cards remaining
		if (cards[i]) 
		{
			delete cards[i];
			cards[i]=NULL;
		}
	delete cards; //Delete the cards pointers
	cards = NULL;

	
}