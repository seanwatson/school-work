/*************************************************************************
 *
 * File: 	 lib_ins_and.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements logical AND as a synthetic instruction for the Western2208
 *
 * Formula:	AND(a,b) = NOT(OR(NOT(a),NOT(b)))
 *
 * Temporary Registers Modified: %t1, %t2
 *
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "and";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){
	
	//check for the proper number of operands, print error otherwise
	if(i.operand_count == 3){

		//implement the instruction using the formula provided
		emit(pass, ins("mov", 2, i.operands[1], reg(REG_TMP2)));
		emit(pass, ins("not", 2, i.operands[0], reg(REG_TMP1)));
		emit(pass, ins("not", 1, reg(REG_TMP2)));
		emit(pass, ins("or", 3, reg(REG_TMP1), reg(REG_TMP2), i.operands[2]));
		emit(pass, ins("not", 1, i.operands[2]));
	}
	else{
		fprintf(stderr, "Invalid number of arguments for 'and'\n");
		exit(EXIT_FAILURE);
	}
}
		
