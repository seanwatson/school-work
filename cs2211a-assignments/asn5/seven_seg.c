#include <stdio.h>
#include <stdbool.h>

#define MAX_DIGITS 20
#define ASCII_OFFSET 48

char segments[10][7] = {{'_','|','|','_','|','|',' '},
				{' ','|','|',' ',' ',' ',' '},
				{'_','|',' ','_','|',' ','_'},
				{'_','|','|','_',' ',' ','_'},
				{' ','|','|',' ',' ','|','_'},
				{'_',' ','|','_',' ','|','_'},
				{'_',' ','|','_','|','|','_'},
				{'_','|','|',' ',' ',' ',' '},
				{'_','|','|','_','|','|','_'},
				{'_','|','|',' ',' ','|','_'}};
char digits[3][MAX_DIGITS * 4];

void clear_digits_array(void){
	int i, j;
	for(i = 0; i < 3; i++)
		for (j = 0; j < MAX_DIGITS * 4; j++)
			digits[i][j] = ' ';
	return;
}

void process_digit(int digit, int position){
	int offset = 4 * position;
	digits[0][offset + 1] = segments[digit][0];
	digits[1][offset] = segments[digit][5];
	digits[1][offset + 1] = segments[digit][6];
	digits[1][offset + 2] = segments[digit][1];
	digits[2][offset] = segments[digit][4];
	digits[2][offset + 1] = segments[digit][3];
	digits[2][offset + 2] = segments[digit][2];
	return;
}

void print_digits_array(void){
	int i, j;
	for(i = 0; i < 3; i++){
		for(j = 0; j < MAX_DIGITS * 4; j++)
			printf("%c", digits[i][j]);
		printf("\n");
	}
	return;
}

int main(void){
	

	clear_digits_array();

	int digit[20];
	int position = 0;

	printf("Enter a number: ");

	int count = scanf("%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u%1u",
digit[0],
digit[1],
digit[2],
digit[3],
digit[4],
digit[5],
digit[6],
digit[7],
digit[8],
digit[9],
digit[10],
digit[11],
digit[12],
digit[13],
digit[14],
digit[15],
digit[16],
digit[17],
digit[18],
digit[19]);

	while( count > 0){
		process_digit(digit[20-count], position);
		position++;
		count--;
	}

	print_digits_array();
	printf("\n");
	return 0;
}
