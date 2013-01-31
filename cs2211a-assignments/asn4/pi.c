/*
 *Computes the approximate value of pi
 *
 *Author: Sean Watson
 *Date: Nov. 2, 2011
 */

#include <stdio.h>	//Input output library
#include <stdlib.h>	//Library for using rand()
#include <time.h>	//Library for using time to make rand() different each run
#include <math.h>	//Math library for computing square root

int main(void){

	srand((unsigned) time(NULL));	//Initialize the rand() function with the current time

	int n = 1;
	int inside = 0;
	double pi = 0;

	//Prompt the user for a number of sample points to use
	printf("Please enter the number of points to compute: ");
	scanf("%d", &n);

	//Validate that the number of sample points is a positive integer
	while (n<1){
		printf("Invalid input. Please enter a positive integer: ");
		scanf("%d", &n);
	}

	int i = 0;
	
	//Compute the number of sample points specified by the user
	for (i; i < n; i++){

		double x = (double)rand()/(double)RAND_MAX;	//Assign a random value between 0 and 1 to x
		double y = (double)rand()/(double)RAND_MAX;	//Assign a random value between 0 and 1 to y
		double r =  sqrt((x*x)+(y*y));			//Compute the square root
		
		//If the random point is in the unit circle, increment the counter
		if (r <= 1){
			inside += 1;
		}
	}

	//Compute the value of pi using the given formula
	pi = 4 * ((double)inside/(double)n);

	//Print the result
	printf("Using %d samples, the value of pi = %e\n", n, pi);

	return 0;
}

