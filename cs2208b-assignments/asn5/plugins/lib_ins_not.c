/*************************************************************************
 *
 * File: 	 lib_ins_not.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements logical NOT as a synthetic instruction for the Western2208.
 * If one operand is given the result is stored in the same register as
 * the source. If two operands are given the result is stored in the
 * second operand
 *
 * Formula:	NOT(a) = NOR(a,a)
 *
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "not";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){
	
	//if there is one operand store the result back to the source
	if(i.operand_count == 1){
		emit(pass, ins("nor", 3, i.operands[0], i.operands[0], i.operands[0]));
	}

	//if there are two operands store the result in the second
	else if (i.operand_count == 2){
		emit(pass, ins("nor", 3, i.operands[0], i.operands[0], i.operands[1]));
	}

	//print an error if there are any other number of operands
	else{
		fprintf(stderr, "Invalid number of arguments for 'not'\n");
		exit(EXIT_FAILURE);
	}
}
		
