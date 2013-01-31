/*
 *Compute and display the magic square of a given size
 *
 *Author: Sean Watson
 *Date: Nov. 2, 2011
 */

#include <stdio.h>	//Include for input and output

int main(void){

	int size=0;

	//Prompt the user for the size of the square
	printf("Enter size of magic square: ");
	scanf("%d",&size);

	//Validate the input was an odd number between 1 and 99
	while (size%2 == 0 || size < 1 || size > 99){
		printf("Invalid input. Please input an odd integer between 1 and 99: ");
		scanf("%d", &size);
	}

	//Initialize where the first entry will go
	int col = size/2;
	int row = 0;

	//Initialize a 2D array for the square
	int square[size][size];
	
	int i = 0, j = 0;
	
	//Initialize the array with all 0s
	for (i; i < size; i++){
		j = 0;
		for (j; j < size; j++){
			square[i][j]=0;
		}
	}

	i = 1;

	//Place every value from 1 to size^2 into the arry
	for (i; i <= size*size; i++){

		if (col == size){ col=0; }	//Wrap the column around if it is outside the array
		if (row == -1){ row=size-1; }	//Wrap the row around if it is outside the array
	
		//Check if the spot is already filled it
		if (square[row][col] != 0) {
			row += 2;	//If it is filled go to the spot beneath the last entry
			col -= 1;
			if (col < 0){ col=size-1; }	//Wrap the column if it is outside the array
			if (row >= size){ row=1; }	//Wrap the row if it is outside the array
		}
		
		square[row][col] = i;	//Insert the current value into the proper spot in the array

		col += 1;	//Move to the next column to the right
		row -= 1;	//Move to the next row up

	}

	//Print the square
	i = 0; j = 0;
	for (i; i < size; i++){
		j = 0;
		for ( j; j < size; j++){
			printf("%d\t", square[i][j]);
		}
		printf("\n");
	}

	return 0;
}
