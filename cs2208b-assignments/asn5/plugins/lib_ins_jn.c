/*************************************************************************
 *
 * File: 	 lib_ins_jn.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements JN as a synthetic instruction for the Western2208. This will
 * cause execution to jump to the specified label if the negative flag is
 * set
 *
 * Formula:	%pc = OR(AND(_label,%fn),AND(FFFFFFFF,NOT(%fn)))
 *
 * Temporary Registers Modified: %t3, %t4
 *
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "jn";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){
	
	//check for proper operand structure, print an error if wrong
	if(i.operand_count == 1 && i.operands[0].type == OP_LABEL){

		//implement the formula
		emit(pass, ins("not", 2, reg(REG_FN), reg(REG_TMP3)));
		emit(pass, ins("not", 2, reg(REG_R0), reg(REG_TMP4)));
		emit(pass, ins("and", 3, reg(REG_TMP3), reg(REG_TMP4), reg(REG_TMP3)));
		emit(pass, ins("and", 3, reg(REG_FN), i.operands[0], reg(REG_TMP4)));
		emit(pass, ins("or", 3, reg(REG_TMP3), reg(REG_TMP4), reg(REG_TMP3)));
		emit(pass, ins("mov", 2, reg(REG_TMP3), reg(REG_PC)));
	}
	else{
		fprintf(stderr, "Invalid arguments for 'jn'\n");
		exit(EXIT_FAILURE);
	}
}
		
