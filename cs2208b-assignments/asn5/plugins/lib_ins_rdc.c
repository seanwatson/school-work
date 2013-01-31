/*******************************************************************************
 *
 * File         : lib_ins_rdc.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * Implements the 'rdc' synthetic instruction (read char). This moves the 
 * value REQ_READ_CHAR (defined in western2208.h) into the I/O register, which 
 * tells the CPU to wait for the user to enter a character, which will be 
 * stored in register %r1.
 * 
 ******************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>

char mnemonic[] = "rdc";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)) 
{
	if (i.operand_count != 0)
	{
		fprintf(stderr, "Invalid arguments for 'rdc'\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		emit(pass, ins("mov", 2, imm(REQ_READ_CHAR), reg(REG_IO)));
	}
	
}
