/*******************************************************************************
 *
 * File         : assembler_plugin.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * See function descriptions in assembler_plugin.h.
 * 
 ******************************************************************************/

#include "assembler_plugin.h"

operand_t reg(reg_t reg)
{
	operand_t operand;
	
	operand.type = OP_REGISTER;
	operand.value.integer = reg;
	
	return operand;
}

operand_t imm(int value)
{
	operand_t operand;
	
	operand.type = OP_IMMEDIATE;
	operand.value.integer = value;
	
	return operand;
}

instruction_t ins(char* mnemonic, int operand_count, ...)
{
	instruction_t instruction;
	va_list ops;
	int i;
    
	if (operand_count > 3)
		operand_count = 3;
		
	instruction.mnemonic = mnemonic;
	instruction.operand_count = operand_count;
	
	va_start(ops, operand_count);
	
	for (i = 0; i < operand_count; i++)
		instruction.operands[i] = va_arg(ops, operand_t);
	
	return instruction;
}
