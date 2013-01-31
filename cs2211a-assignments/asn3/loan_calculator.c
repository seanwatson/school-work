/*
 *Program to compute monthly loan payments
 *
 *Author: Sean Watson
 *Date: Oct. 25, 2011
 */

#include <stdio.h>	//Include the in/out library for reading input and printing output

int main(void){

	//Initialize variables to store the loan, interest rate, and payments
	float loan, interest, payment;
	int n;

	//Prompt for the amount of the loan
	printf("Please enter the amount of the loan:");
	scanf("%f", &loan);

	//Verify the loan is a positive amount
	while (loan < 0){
		printf("Invalid loan amount. Please enter a positive value:");
		scanf("%f", &loan);	//Prompt for another amount if it is negative
	}

	//Prompt for the yearly interest rate
	printf("Please enter the yearly interest rate:");
	scanf("%f", &interest);

	//Verify the interest is a positive value
	while (interest < 0){
		printf("Invalid interest rate. Please enter a positive value:");
		scanf("%f", &interest);	//Prompt for another value if it is negative
	}

	//Prompt for the monthly payment amount
	printf("Please enter the monthly payment amount:");
	scanf("%f", &payment);

	//Verify the payment is a positive value
	while (payment < 0){
		printf("Invalid payment amount. Please enter a positive value:");
		scanf("%f", &payment);	//Prompt for another amount if the value is negative
	}

	//Prompt for the number of months to do calculations for
	printf("Please enter the number of months to calculate:");
	scanf("%d", &n);

	//Verify the number of months is a positive value
	while (n < 0){
		printf("Invalid months value. Please enter a positive value:");
		scanf("%d", &n);	//Prompt for another value if the number of months is negative
	}

	//Divide yearly interest percentage to get monthly interest value
	interest /= 1200;

	//Do the computation the number of times specified by the number of months input
	int i;
	for (i=1; i<=n; i++){

		//Calculate the new value if the loan
		loan = loan - payment + (loan * interest);

		//If the value is negative, determine the final payment
		if (loan < 0){
			loan = loan + payment;

			//Print the final payment
			printf("The final payment will be %.2f after %d months\n", loan, i);
			break;
		}

		//Print the new value of the loan
		printf("The remaining value of the loan after %d months is $%.2f\n", i, loan);
	}
	return 0;
}
