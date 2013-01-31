/*************************************************************************
 *
 * File: 	 lib_ins_jmp.c
 * Author:	 Sean Watson
 * Last Updated: April 8, 2012
 *
 * Implements JMP as a synthetic instruction for the Western2208. Can be
 * used to jump to a label in the program unconditionally
 *
 * Formula:	%pc = _label
 *
 *************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

//mnemonic for the instruction
char mnemonic[] = "jmp";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)){
	
	//check for proper operand structure, print error if wrong
	if(i.operand_count == 1 && i.operands[0].type == OP_LABEL){

		//move the address corresponding to the label into the program counter
		emit(pass, ins("mov", 2, i.operands[0], reg(REG_PC)));
	}
	else{
		fprintf(stderr, "Invalid arguments for 'jmp'\n");
		exit(EXIT_FAILURE);
	}
}
		
