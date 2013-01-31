/*************************************************************************
 *
 * File: 	 lib_ins_or.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements logical OR as a synthetic instruction for the Western2208.
 *
 * Formula:	OR(a,b) = NOT(NOR(a,b))
 *
 * Temporary Registers Modified: %t0
 *
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "or";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){
	
	//check for the proper number of operands, print an error if wrong number
	if(i.operand_count == 3){

		//implement the formula
		emit(pass, ins("mov", 2, i.operands[1], reg(REG_TMP0)));
		emit(pass, ins("nor", 3, i.operands[0], reg(REG_TMP0), i.operands[2]));
		emit(pass, ins("not", 1, i.operands[2]));
	}
	else{
		fprintf(stderr, "Invalid number of arguments for 'or'\n");
		exit(EXIT_FAILURE);
	}
}
		
