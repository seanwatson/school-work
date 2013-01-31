/*************************************************************************
 *
 * File: 	 lib_ins_neg.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements NEG as a synthetic instruction for the Western2208. This
 * negates the parameter. The result can be stored in the same register
 * or a different register provided as a second operand.
 *
 * Formula:	NEG(a) = NOT(a) + 1
 *
 * Temporary Registers Modified: %t0
 * 
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "neg";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){

	//store result in same register if one operand
	if(i.operand_count == 1){
		emit(pass, ins("not", 1, i.operands[0]));
		emit(pass, ins("add", 3, i.operands[0], imm(1), i.operands[0]));
	}

	//store result in second parameter register if two operands
	else if (i.operand_count == 2){
		emit(pass, ins("not", 2, i.operands[0], reg(REG_TMP0)));
		emit(pass, ins("add", 3, reg(REG_TMP0), imm(1), i.operands[1]));
	}

	//print error if any other amount of registers
	else{
		fprintf(stderr, "Invalid number of arguments for 'neg'\n");
		exit(EXIT_FAILURE);
	}
}
		
