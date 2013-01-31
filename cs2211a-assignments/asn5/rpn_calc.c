#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACK_SIZE 100
#define ASCII_OFFSET 48

int contents[STACK_SIZE];
int top = 0;

bool is_full(void){
	return top == STACK_SIZE;
}

bool is_empty(void){
	return top == 0;
}

void push(int i){
	if (is_full()){
		printf("Expression is too complex. Terminating program.\n");
		exit(0);
	}
	else{
		contents[top++] = i;
	}
}

int pop(void){
	if (is_empty()){
		printf("Not enough operands. Terminating program.\n");
		exit(0);
	}
	else{
		return contents[--top];
	}
}

int main(void){

	char ch = 'a';

	while (1){
		
		printf("Enter an RPN expression: ");
		scanf(" %c", &ch);
		

		while (1){

			if (ch >= '0' && ch <= '9'){
				push(((int) ch) - ASCII_OFFSET);
			}
			else if (ch == '+'){
				int right = pop();
				int left = pop();
				push(left + right);
			}
			else if (ch == '-'){
				int right = pop();
				int left = pop();
				push(left - right);
			}else if (ch == '*'){
				int right = pop();
				int left = pop();
				push(left * right);
			}else if (ch == '/'){
				int right = pop();
				int left = pop();
				if (right == 0){
					printf("Error. Divides by 0. Terminating program.\n");
					return 0;					
				}
				push(left / right);
			}else if (ch == '='){
				int result = pop();
				if (is_empty()){
					printf("%d\n", result);
				}
				else{
					printf("Error. Tried to display result when stack was not empty.\n");
					while (!is_empty()){
						pop();
					}
				}
				break;
			}
			else return 0;
			scanf(" %c", &ch);
		}
	}
}
