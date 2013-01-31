#include "Point.h"

int main(){

	Point c[4]; //Create array of 4 point objects
	c[1]= Point(7, 7); //Define the second point in the array to hold 7, 7
	c[2]= Point(6, 5); //Define the third point in the array to hold 6, 5
	Point *b[3]; //Create an array of 3 point pointers
	b[0]= c + 2; //First pointer holds address of third point
	b[1]= NULL; //Second pointer holds null value
	b[2]= new Point[3]; //Third pointer creates dynamic array of 3 point objects
	b[2][0]= Point(1, 1); //First dynamic point holds 1, 1
	b[2][2]= Point(2,3); //Third dynamic point holds 2, 3
	Point **d= &b[2]; //Create a point pointer pointer that holds the address of the third pointer
	Point *a= &b[2][1]; //Create a point pointer which holds the adress of the second dynamic pointer
	
	a= c+2; //Change a to hold the address of the third static point
	delete [] b[2]; //Delete the dynamic array
	b[2]= c+3; //Change the third pointer to the adress of the fourth static point
	b[0]= c+1; //Change the first pointer to the address of the second point
	d= NULL; //Change d to null value

	return 0;

}
