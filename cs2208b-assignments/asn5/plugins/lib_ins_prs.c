/*******************************************************************************
 *
 * File         : lib_ins_prs.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * Implements the 'prs' synthetic instruction (print string). This moves the 
 * value REQ_PRINT_STR (defined in western2208.h) into the I/O register, which 
 * tells the CPU to print the string at the address stored in register %r1.
 * 
 ******************************************************************************/

#include "assembler_plugin.h"
#include <stdio.h>

char mnemonic[] = "prs";

void handle(int pass, instruction_t i, void (*emit)(int, instruction_t)) 
{
	if (i.operand_count != 0)
	{
		fprintf(stderr, "Invalid arguments for 'prs'\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		emit(pass, ins("mov", 2, imm(REQ_PRINT_STR), reg(REG_IO)));
	}
	
}
