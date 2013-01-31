/*
 *A program to evaluate the value of "e" to a specified precision
 *
 *Author: Sean Watson
 *Date: Oct. 25, 2011
 */


#include <stdio.h>	//Include the in/out library for reading input and printing output

int main(void){

	//Initialize variables to hold the limit, current term, and value of e
	float limit=0, term=1.0, e=1.0;

	//Initialize a counter to keep track of the number of terms calculated
	int counter=1;

	//Prompt for a lower limit of the value of a term
	printf("Please enter a lower limit on the calculated term:");
	scanf("%f", &limit);

	//Validate that the lower limit is greater than 0
	while (limit <= 0){
		printf("Please enter a positive value for the lower limit:");
		scanf("%f", &limit);
	}

	//Keep summing terms while the current term is greater than the limit
	while (term > limit){

		float i = 1;
		term = 1.0;	//Reset the current term to 1.0 so it can be divided by the factorial
		counter++;	//Increment the counter of terms
		for (i=1; i<counter; i++){	
			term /= i;	//Divide by the factorial of the current term
		}
		e += term;	//Add the value of the current term to the e value
	}

	//Print the value of e and the number of terms needed to caluculate it
	printf("The value of e is %.15f after %d terms\n", e, counter);
	return 0;
}
