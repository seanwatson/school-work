#include "cs1037util.h"
#include <iostream>
#include <time.h>
#include "Bag.h"
#include "Stack.h"
#include "List.h"
#include "ImageCard.h"

using namespace std;

// global variables
Stack<ImageCard*> bank(36); // a stack of bank cards
List<ImageCard*> user_hand(20); // a list of cards in the user's hand
List<ImageCard*> comp_hand(20); // a list of cards in the computer's hand
List<List<ImageCard*>*> trick_pairs(6); // a list of tricks where each
                                        // "trick" is a list of 
									    // (up to two) cards: "lead" (1) and "beat" (2).

enum RoundType {USER_LEADS=0, COMP_LEADS=1} mode; // a variable to store the mode of the current round of the game
// C++ allows to define special "user-defined" types of integers where values 0,1,2,... are
// replaced with special text-tags verbally describing interpretation of each value. This helps to write
// more clear and intuitive code.


// YOU CAN DECLARE NECESSARY GLOBAL VARIABLES HERE (DO NOT USE MORE VARIABLES THAN WHAT IS ABSOLUTELY NECESSARY)
//....
char trump;         //variable to store trump suit
unsigned int selection; //variable to store right click selection



//global functions/methods: DO NOT DECLARE ANY OTHER GLOBAL FUNCTIONS
void clear_cards();
void init_cards();
void draw_cards();
void leftClick(Point p); // These three functions are called when a user mouse clicks
void rightClick(Point p);   
void rightUnClick(Point p); 

void returnKey(); // This function is called by the "main" when the user hits "Return"
void setNewRound(); // Your code for 'setNewRound' should be used to process tricks at the end
                    // of each round, replenish the hands with cards, and to set-up the next Round.
bool checkLead(ImageCard * card); // two methods for checking validity of 
bool checkBeat(ImageCard * card); // a "lead" or a "beat" card based
                                            // on the current "tricks" list
bool addLead(ImageCard * card); // two methods for adding a card to
bool addBeat(ImageCard * card); // to the "tricks" list. addLead() adds
							     // card to a new (open) trick while setBeat() 
								 // completes (closes) an existing open trick.
ImageCard * getCompLead(); // methods for computing "automatic" moves 
ImageCard * getCompBeat(); // of the computer's hand. You can implement these
                            // methods based on a really dumb strategy: 
                            // loop over cards in the game's hand until 
                            // a valid "lead" (or "beat") card is found
                            // Use checkLead() or checkBeat() functions 
                            // to determine validity of a move. This card
                            // should be removed from the game's hand and 
                            // send as a return card. Return NULL if 
			       		    // no valid "lead" (or "beat") is found.

void replenishCompHand(); // These methods move cards from the bank list
void replenishUserHand(); // into the user's or the game's hand at the 
                          // beginning of each new round. 




// the "main" function: DO NOT CHANGE CODE FOR "MAIN" ITSELF !!!!!!!!!!!!!
int main()
{
	cout << " 'q' - to quit\n 'n' - to start a new game" << endl;
	cout << " left mouse clicks - select your card to lead/beat" << endl;
	cout << " right mouse clicks/unclicks - sort cards in your hand" << endl;
	cout << " hit 'ENTER' to finish a round" << endl << endl;

	srand( (unsigned) time( NULL ) ); // initialize random number generator used in Bags<Item>
	init_cards();
	draw_cards();
	SetWindowTitle("Kozel Game"); // see "cs1037utils.h"
	SetWindowSize(1000,400); 
	SetWindowVisible(true); 

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
			else if (c == '\r')             // user hits 'ENTER' 
			{
				returnKey();
				draw_cards();
			}
		}

		// READING MOUSE INPUT, IF ANY
		int x, y, button;
		if (GetMouseInput(&x, &y, &button)) // check mouse 
		{
			if (button == 1)                // button 1 means the left button
			{	
				leftClick(Point(x,y));
				draw_cards();
			}
			else if (button == 2)                // button 2 means the right button click
			{	
				rightClick(Point(x,y));
				draw_cards();
			}
			else if (button == -2)                // button -2 means the right button unclick
			{	
				rightUnClick(Point(x,y));
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

	// drawing the cards in the bank (since bank is a stack you may need to move cards temporarily into a
	// different data structure and then return them back once finished drawing.
	// ...
	SetDrawColour(0.0,0.0,0.0); //Set draw colour to black
	DrawString(15, 260, "Bank");
	List<ImageCard*> temp(bank.getSize()); //Temporary list to draw bank with
	while (!bank.isEmpty()) //Move the bank cards to the temp list until its empty
		temp.append(bank.pop());
	if (!temp.isEmpty()) //Make sure temp has at least one card
	{
		temp.back()->drawAt(Point(20,150)); //Draw trump card and put it back to the bank
		bank.push(temp.popBack());
	}
	for (int i=0; !temp.isEmpty(); i++)
	{
		temp.back()->drawAt(Point(50+i,150+(i/5))); //Draw the rest of the cards and put them back until list is empty
		bank.push(temp.popBack());
	}

	// drawing the tricks (pairs of cards in the trick list)
	// ...
	for (unsigned i=1; i<=trick_pairs.getLength(); i++) //To cycle through the list of lists
		for (unsigned j=1; j<=trick_pairs.retrieve(i)->getLength(); j++) //To cylce through each card in a list
			trick_pairs.retrieve(i)->retrieve(j)->drawAt(Point(90+(100*i)+(j*15),145+(j*5))); //Draw each card according to 
																							  //which list and card number it is
	// drawing the cards in the game hand list
	// ...
	DrawString(40, 18, "Computer's Hand");
	for (unsigned i=1; i<=comp_hand.getLength(); i++) //Draw the cards going down the list
		comp_hand.retrieve(i)->drawAt(Point(100+((CARD_WIDTH+5)*i),25));


	// drawing the cards in the user hand list
	// ...
	DrawString(75, 365, "Your Hand"); //Draw cards going down the list
	for (unsigned i=1; i<=user_hand.getLength(); i++)
		user_hand.retrieve(i)->drawAt(Point(100+((CARD_WIDTH+5)*i),275));
}

void init_cards()
{
	mode=USER_LEADS; // The first round of the game is set into "USER_LEADS" mode

	// Create a bag object to hold 36 cards (you can use a static one) and 
	// add to it 36 dynamically created cards (starting from denomination 6 
	// in each suit). 
	// ...
	Bag<ImageCard *> deck(36);
	deck.add( new ImageCard(HEARTS,ACE) );  //Rediculously inefficient way to create a deck
	deck.add( new ImageCard(HEARTS,KING) );
	deck.add( new ImageCard(HEARTS,QUEEN) );
	deck.add( new ImageCard(HEARTS,JACK) );
	deck.add( new ImageCard(HEARTS,N10) );
	deck.add( new ImageCard(HEARTS,N9) );
	deck.add( new ImageCard(HEARTS,N8) );
	deck.add( new ImageCard(HEARTS,N7) );
	deck.add( new ImageCard(HEARTS,N6) );
	deck.add( new ImageCard(DIAMONDS,ACE) );
	deck.add( new ImageCard(DIAMONDS,KING) );
	deck.add( new ImageCard(DIAMONDS,QUEEN) );
	deck.add( new ImageCard(DIAMONDS,JACK) );
	deck.add( new ImageCard(DIAMONDS,N10) );
	deck.add( new ImageCard(DIAMONDS,N9) );
	deck.add( new ImageCard(DIAMONDS,N8) );
	deck.add( new ImageCard(DIAMONDS,N7) );
	deck.add( new ImageCard(DIAMONDS,N6) );
	deck.add( new ImageCard(SPADES,ACE) );
	deck.add( new ImageCard(SPADES,KING) );
	deck.add( new ImageCard(SPADES,QUEEN) );
	deck.add( new ImageCard(SPADES,JACK) );
	deck.add( new ImageCard(SPADES,N10) );
	deck.add( new ImageCard(SPADES,N9) );
	deck.add( new ImageCard(SPADES,N8) );
	deck.add( new ImageCard(SPADES,N7) );
	deck.add( new ImageCard(SPADES,N6) );
	deck.add( new ImageCard(CLUBS,ACE) );
	deck.add( new ImageCard(CLUBS,KING) );
	deck.add( new ImageCard(CLUBS,QUEEN) );
	deck.add( new ImageCard(CLUBS,JACK) );
	deck.add( new ImageCard(CLUBS,N10) );
	deck.add( new ImageCard(CLUBS,N9) );
	deck.add( new ImageCard(CLUBS,N8) );
	deck.add( new ImageCard(CLUBS,N7) );
	deck.add( new ImageCard(CLUBS,N6) );

	// Move all cards from your bag into "bank". This is equivalent to putting 
	// a shuffled deck of cards into the bank. Then call "replenish...Hand()" functions to fill 
	// up the user's and the computer's hands from the bank (Note that "replenish...Hand()" functions 
	// should be implemented by you (see below) before they actually can do something) 
	// NOTE: Set the first card in the bank "face up" and copy its "suit" into "trump_suit" variable.  
	// (draw_cards() will draw the first card of the bank as a "trump" card.)
    // ...
	bank.push(deck.getOne());
	bank.peek()->setFaceUp();
	trump = bank.peek()->toCharSuit();
	while (!deck.isEmpty())
		bank.push(deck.getOne());
	replenishUserHand();
	replenishCompHand();


	// Print instructions on your console
	cout << endl << "Welcome to NEW Kozel Game!" << endl << endl;
	cout << "Lead your cards." << endl;
}

void clear_cards()
{
	// deleting all cards in the bank, user_hand, and game_hand
    // ...
	while (!bank.isEmpty())
		delete bank.pop();
	while (!user_hand.isEmpty())
		delete user_hand.remove(1);
	while (!comp_hand.isEmpty())
		delete comp_hand.remove(1);
	for (unsigned i=1; i<=trick_pairs.getLength(); i++) //Delete any cards in the trick pair lists
		while (!trick_pairs.retrieve(i)->isEmpty())
			delete trick_pairs.retrieve(i)->remove(1);
	while (!trick_pairs.isEmpty())
		delete trick_pairs.remove(1); //Delete the list pair tricks
}

// These following two functions are called by "main"
// when the user right "clicks" (to select a card), holds, 
// and then "unclicks" (to indicate its new location in the hand). 
// The user hand sorting implemented by the next two functions
// is completely independent from the rest of the game.
void rightClick(Point p)
{
// ...
	for (unsigned i=1; i<=user_hand.getLength(); i++) //Look through every card
		if (user_hand.retrieve(i) && user_hand.retrieve(i)->pointInCard(p)) //See if it is the one clicked
		{
			selection = i; //If it is, store the position in the hand in selection
			break; //No sense looking through other cards once one is found
		}
}

// This function moves selected card into the new position/index 
// in the user hand list
void rightUnClick(Point p)
{
// ....
	for (unsigned i=1; i<=user_hand.getLength(); i++) //Look through all the cards
		if (user_hand.retrieve(i) && user_hand.retrieve(i)->pointInCard(p)) //Find the one unclicked
		{
			if (i>selection)
				while(selection!=i)
				{
					user_hand.swap(selection, selection+1); //If its moved to the right, swap the card to the right until its in the right position
					selection++;
					
				}
			else
				while(selection!=i)
				{						//If its moved to the left, swap it to the left until its in the right position
					user_hand.swap(selection, selection-1); 
					selection--;
				}
			break; //No sense looking through the rest of the cards once it is moved
		}

}

// leftClick() is a critical function that implements user's moves
// (as he/she selects a "lead" or a "beat" card) and the corresponding response
// of the game's hand during the game. "main" calls this function each time 
// when the user left-clicks on some point p on the screen (in pixel coordinates
// in the game's window). 
void leftClick(Point p)
{
	if (mode==USER_LEADS)
	{
		// find the card in user's hand that has been clicked on (loop over the cards 
		// in the user's hand list until you find one that contains p, use pointInCard(p)).
		// If you find one, check if it is a valid "Lead" (use checkLead() function).
		// If yes, remove it from the user's hand list and use addLead() to move 
		// it into the tricks list. Call draw_cards() and Pause() to update the display.
		// Then, use getCompBeat() to find games's hand "beat" card, 
		// if any. Use addBeat() to add it into the trick's list, or 
		// setNewRound() if game's hand found no "beat". 
		// HINT: "Pause" function gives time for the display to refresh and also imitates 
		// some "thinking time" between the calls to addLead(), addBeat(), e.t.c.
		// ...

		for (unsigned i=1; i<=user_hand.getLength(); i++) 
			if (user_hand.retrieve(i) && user_hand.retrieve(i)->pointInCard(p)) //find card clicked
				if (checkLead(user_hand.retrieve(i))) //See if it is a legal lead
				{
					addLead(user_hand.remove(i)); //add it to the trick pairs
					draw_cards();
					Pause(1000);
					bool comp_beat=false; //declare local variable to see if comp has a legal beat
					for (unsigned i=1; i<=comp_hand.getLength(); i++)
						if (checkBeat(comp_hand.retrieve(i))) //Check all cards for a beat
						{
							comp_beat=true;
							break;
						}
					if (comp_beat)
						addBeat(getCompBeat()); //if there was a beat, add it to the tricks
					else
						setNewRound(); //if not end the round
					draw_cards();
					break;
				}

	}
	else if (mode==COMP_LEADS)
	{
		// find the card in user's hand that has been clicked on.
		// If you find one, check if it is a valid "Beat" (use checkBeat()).
		// If yes, remove it from the user's hand list and use addBeat() to move 
		// it into the tricks list. Then, use getCompLead() to find computer's hand next "lead" card, 
		// if any. Use addLead() to add it into the trick's list, or 
		// setNewRound() if game's hand found no new "lead".
		// HINT: Use draw_cards() and Pause() to keep the display updated.
		// ...
		
		for (unsigned i=1; i<=user_hand.getLength(); i++) 
			if (user_hand.retrieve(i) && user_hand.retrieve(i)->pointInCard(p)) //find the card clicked
				if (checkBeat(user_hand.retrieve(i))) //see if it is a legal beat
				{
					addBeat(user_hand.remove(i)); //if so, add it to the trick
					draw_cards();
					Pause(2000);
					bool comp_lead=false; //local variable to see if comp has a lead
					for (unsigned i=1; i<=comp_hand.getLength(); i++)
						if (checkLead(comp_hand.retrieve(i))) //check all cards for a legal lead
						{
							comp_lead=true;
							break;
						}
					if (comp_lead)
						addLead(getCompLead()); //if there is a lead, add it to the tricks, if not end the round
					else
						setNewRound();
					draw_cards();
					break;
				}
	}
}

// This function should add a new (dynamically created) 
// trick (stack of KozelCards*) at the end of
// 'trick_pairs' (a list of such stacks) and 
// add a card (a "lead") to this new stack.
// Also, set the card "faceUp".
bool addLead(ImageCard * card)
{
	trick_pairs.append(new List<ImageCard*>(2)); //Add a new trick to the list
	if (!card->isFaceUp()) //make sure the card is face up when it is added
		card->setFaceUp();
	trick_pairs.back()->append(card); //put the card in the last trick in the list
	return true;
}

// This function should add a second card (a "beat") into 
// the last trick in the list of tricks.
// Set the card "faceUp".
bool addBeat(ImageCard * card)
{
	if (!card->isFaceUp()) //make sure the card being added is face up
		card->setFaceUp();
	trick_pairs.back()->append(card); //add it to the last trick in the list
	return true;
}

// A user clicks "RETURN" to indicate that he/she does
// not have a card to continue the current round of the game.
void returnKey() {setNewRound();}

// Function SetNewRound() should wrap-up a finished round of the game
// following the rules if the game.
void setNewRound()
{ 
	// "return" (do nothing) if the trick list is empty
	// ...
	if (trick_pairs.isEmpty())
		return;

	// determine if the previous round was successful
	// (all "leads" were "beat"). That is,
	// check if the last "trick" in the m_trick_list
	// contains two cards. If yes, set a boolean variable
	// "success" to true. Otherwise, set "success" to false;
	// ...
	bool success;
	if (trick_pairs.back()->getLength()==2)
		success=true;
	else
		success=false;
	// Remove all cards stored in the trick_list and put them
	// into a local stack object "collector".
	// 
	Stack <ImageCard*> collector;
	for (unsigned i=1; i<=trick_pairs.getLength(); i++)
		while (!trick_pairs.retrieve(i)->isEmpty())
			collector.push(trick_pairs.retrieve(i)->remove(1));
	while (!trick_pairs.isEmpty()) //remove the lists that held the tricks
		delete trick_pairs.remove(1);


	// Discard the cards in the "collector" if the last round was a "success"
	// Otherwise, insert the cards from the "collector" into the user_hand
	// if the mode of the current round was "COMP_LEADS", or into the game_hand
	// if the mode was "USER_LEADS".
	// ...
	if (success)
		while (!collector.isEmpty())
			delete collector.pop(); //if it was a success, delete the tricks from the last round
	else if (mode==COMP_LEADS)
		while (!collector.isEmpty())
			user_hand.append(collector.pop()); //add the trick cards to the player who couldnt beat
	else
		while (!collector.isEmpty())
		{
			collector.peek()->setFaceDown();
			comp_hand.append(collector.pop());
		}

	draw_cards();
	Pause(2000);

	// Call replenish..Hand() functions to fill-up the user's and the computer's hands.
	// Note that the hand that "lead" in the finished round has to replenish first.
	// ...
	cout << "Replenishing hands with up to 6 cards from the bank" << endl << endl;
	if (mode==USER_LEADS)
	{
		replenishUserHand();
		replenishCompHand();
	}
	else
	{
		replenishCompHand();
		replenishUserHand();
	}

	draw_cards();
	Pause(2000);

	// If one of the hands remains empty, the game is over.
	// Print some text on the screen and return;
	// ...
	if (comp_hand.isEmpty()||user_hand.isEmpty())
	{
		if (comp_hand.isEmpty())
			cout << "Game Over. Computer wins. Sorry :(" << endl;
		else
			cout << "Game Over. You win! Congradulations :)" << endl;
		return;
	}

	// Set the "mode" of the new round depending on "mode" and "success"
	// of the previous round. If "success" is true than the mode should change.
	// Otherwise it stayes the same. Print some 
	// information on the screen describing the mode of the new round.
	//...
	if (success)
	{
		if (mode==USER_LEADS)
		{
			mode=COMP_LEADS;
			cout << "Computer won the last round. Computer leads." << endl;
		}
		else
		{
			mode=USER_LEADS;
			cout << "You won the round. Lead a card." << endl;
		}
	}
	else
	{
		if (mode==USER_LEADS)
			cout << "You won the round. Lead a card." << endl;
		else
			cout << "Computer won the round. Computer leads." << endl;
	}

	// If the new mode is "COMP_LEADS" you should implement the first "lead" 
	// of the game's hand by calling getCompLead() and addLead() functions.
	// ...
	if (mode==COMP_LEADS)
	{
			addLead(getCompLead());
			draw_cards();
	}
	return;
}

// Use a for-loop to find a first valid "lead" card (use checkLead())
// in comp_hand. Remove it from the list and return it.
// Return NULL if no valid "lead" is found.
// If you like, you can implement a "smarter" strategy where the game hand 
// returns the least valuable valid "lead" card in the hand.
ImageCard * getCompLead()
{
	for (unsigned i=1; i<=comp_hand.getLength(); i++)
		if (checkLead(comp_hand.retrieve(i)))
			return comp_hand.remove(i);
	return NULL;
}

// Use a for-loop to find a first valid "beat" card (use checkBeat())
// in comp_hand. Remove it from the list and return it.
// Return NULL if no valid "lead" is found.
// If you like, you can implement a "smarter" strategy where the game hand 
// returns the least valuable valid "beat" card in the hand.
ImageCard * getCompBeat()
{
	for (unsigned i=1; i<=comp_hand.getLength(); i++)
		if (checkBeat(comp_hand.retrieve(i)))
			return comp_hand.remove(i);
	return NULL; 
}

// move cards from the end of the bank list into the end of the computer's hand
// untill there are at least 6 cards in the hand, or until the bank gets empty.
void replenishCompHand()
{
	while (comp_hand.getLength()<6 && !bank.isEmpty())
	{
		comp_hand.append(bank.pop());
		comp_hand.back()->setFaceDown();
	}
		
	
}

// move cards from the end of the bank list into the end of the user's hand
// untill there are at least 6 cards in the hand, or until the bank gets empty.
void replenishUserHand()
{	
	while (user_hand.getLength()<6 && !bank.isEmpty())
	{
		user_hand.append(bank.pop());
		user_hand.back()->setFaceUp();
	}
}

// checks validity of a card for "leading"
// based on the current cards in the tricks list.
bool checkLead(ImageCard * card)
{
	// return false if there are 6 (or more) tricks in the list
	// ...
	if (trick_pairs.getLength()>6)
		return false;

	// return false if the hand that currently "beats" does not have any cards left
	// ...
	if (mode==USER_LEADS)
		if (comp_hand.isEmpty())
			return false;
	if (mode==COMP_LEADS)
		if (user_hand.isEmpty())
			return false;

	// return true if the tricks list does not have any tricks (this is a first "lead" in a new round).
	// ...
	if (trick_pairs.isEmpty())
		return true;

	// Return false if there is at least one "trick" in the list that contains only one card
	// (no new "lead" cards are allowed while there are existing open "leads" (not covered by a "beat")).
    // ...
	else if (trick_pairs.getLength()==1 && trick_pairs.back()->getLength()==1)
		return false;

	// Return true if the 'card' matches in denomination one of the cards in the tricks list and
	// return false otherwise.
	// ...
	else
		for (unsigned i=1; i<=trick_pairs.getLength(); i++)
			for (unsigned j=1; j<=trick_pairs.retrieve(i)->getLength(); j++)
				if (card->getDenomination() == trick_pairs.retrieve(i)->retrieve(j)->getDenomination())
					return true;
	return false;
}

// checks validity of a card for "beating"
// based on the current open (uncovered) "lead" in the tricks list.
bool checkBeat(ImageCard * card)
{
	// return false if there are no tricks (in the trick list)
	// ...
	if (trick_pairs.isEmpty())
		return false;

	// return false if the last trick does not contain precisely one card (an open "lead").
	// ...
	else if (trick_pairs.back()->getLength()!=1)
		return false;
	
	// Return true if the 'card' is more valuable than the open "lead" card.
	// (It should be either a higher denomination card of the same suit
	// or it should be a trump over a non-trump card. Note that denomination 
	// of the cards can be compared using overloaded operators"<" or ">" (see "card.h" file)).
	// Return false otherwise.
	// ...
	else if (card->getSuit() == trick_pairs.back()->back()->getSuit())
		return *card > *(trick_pairs.back()->back());

	else if (card->toCharSuit() == trump)
		return true;
	
	else
		return false;
}
