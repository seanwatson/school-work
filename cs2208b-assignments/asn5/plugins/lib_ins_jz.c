/*************************************************************************
 *
 * File: 	 lib_ins_jz.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements JZ as a synthetic instruction for the Western2208. This will
 * cause execution to jump to the specified label if the zero flag is
 * set
 *
 * Formula:	%pc = OR(AND(_label,%fz),AND(FFFFFFFF,NOT(%fz)))
 *
 * Temporary Registers Modified: %t3, %t4
 *
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "jz";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){
	
	//check for proper operand structure, print error if wrong
	if(i.operand_count == 1 && i.operands[0].type == OP_LABEL){

		//implement the formula
		emit(pass, ins("not", 2, reg(REG_FZ), reg(REG_TMP3)));
		emit(pass, ins("not", 2, reg(REG_R0), reg(REG_TMP4)));
		emit(pass, ins("and", 3, reg(REG_TMP3), reg(REG_TMP4), reg(REG_TMP3)));
		emit(pass, ins("and", 3, reg(REG_FZ), i.operands[0], reg(REG_TMP4)));
		emit(pass, ins("or", 3, reg(REG_TMP3), reg(REG_TMP4), reg(REG_TMP3)));
		emit(pass, ins("mov", 2, reg(REG_TMP3), reg(REG_PC)));
	}
	else{
		fprintf(stderr, "Invalid arguments for 'jz'\n");
		exit(EXIT_FAILURE);
	}
}
		
