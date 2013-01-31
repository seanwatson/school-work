/*************************************************************************
 *
 * File: 	 lib_ins_sub.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements SUB as a synthetic instruction for the Western2208.
 *
 * Formula:	SUB(a,b) = a + NEG(b)
 *
 * Temporary Registers Modified: %t0
 *
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "sub";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){
	
	//check for the proper number of operands, print error if wrong number
	if(i.operand_count == 3){

		//implement the formula
		emit(pass, ins("mov", 2, i.operands[1], reg(REG_TMP0)));
		emit(pass, ins("neg", 1, reg(REG_TMP0)));
		emit(pass, ins("add", 3, i.operands[0], reg(REG_TMP0), i.operands[2]));
	}
	else{
		fprintf(stderr, "Invalid number of arguments for 'sub'\n");
		exit(EXIT_FAILURE);
	}
}
		
