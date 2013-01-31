/*******************************************************************************
 *
 * File         : lib_ins_mov.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 30, 2012
 *
 * Implements the 'mov' synthetic instruction. 
 * 
 ******************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>
#include <stdlib.h>

char mnemonic[] = "mov";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)) 
{
	if (i.operand_count != 2)
	{
		fprintf(stderr, "Invalid arguments for 'mov'\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		emit(pass, ins("add", 3, reg(REG_R0), i.operands[0], i.operands[1]));
	}
	
}
