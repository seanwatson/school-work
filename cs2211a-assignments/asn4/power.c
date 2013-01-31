/*
 *Computes the value of x^n
 *
 *Author: Sean Watson
 *Date: Nov. 2, 2011
 */


#include <stdio.h> //Include the in/out library for input and output

int power(int, int);	//Declare a function to compute the power with

int main(void){

	int x=0;
	int n=0;
	int result=0;

	printf("Please enter a value for x:");	//Prompt the user to enter a base
	scanf("%d", &x);

	printf("Please enter a value for n:");	//Prompt the user to enter a power
	scanf("%d", &n);

	//Make sure the power entered is a positive value
	while (n < 0 ){
		printf("Invalid input. Please enter a positive integer for n: ");
		scanf("%d", &n);
	}

	//If it is an even power, calulate the power of n/2 and square the result
	if (n%2==0){
		result = power(x, n/2);
		result *= result;
	}

	//If it is an odd power, calculate teh power of (n-1)/2, square the result, and times by x
	else {
		result = power(x, (n-1)/2);
		result *= result * x;
	}

	//Print the result
	printf("%d^%d=%d\n", x, n, result);
	return 0;
}

//Function that recursively computes the power given a base and power
int power(int x, int n){
	if (n==0){ return 1;}	//Base case
	return (x*(power(x, n-1)));	//Return x * x^n-1 which equals x^n
}
