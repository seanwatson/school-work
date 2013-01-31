/*******************************************************************************
 *
 * File         : assembler_plugin.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * Helper functions useful to developers implementing plugins for the Western
 * 2208 assembler.
 * 
 ******************************************************************************/

#include "assembler.h"
#include <stdarg.h>
#include <stdlib.h>

#ifndef ASSEMBLER_PLUGIN_H
#define ASSEMBLER_PLUGIN_H

/*******************************************************************************
 * Takes an integer register value and returns an operand_t initialized properly
 * with the register value and the correct operand type.
 *
 * Sample usage:
 *
 * operand_t op = reg(REG_R1) initializes 'op' with an operand_t such that:
 *
 * op.type = OP_REGISTER
 * op.value.integer = REG_R1
 *
 * See the sample plugins for more examples of using this function.
 *
 ******************************************************************************/
operand_t reg(reg_t reg);

/*******************************************************************************
 * Takes an integer value and returns an operand_t initialized properly
 * with the value and the correct operand type.
 *
 * Sample usage:
 *
 * operand_t op = imm(44) initializes 'op' with an operand_t such that:
 *
 * op.type = OP_IMMEDIATE
 * op.value.integer = 44
 *
 * See the sample plugins for more examples of using this function.
 *
 ******************************************************************************/
operand_t imm(int value);

/*******************************************************************************
 * Takes an assembler mnemonic, the number of operands to place in the
 * instruction, and a variable number of operands.
 *
 * Returns an instruction_t initialized with the specified mnemonic and operands.
 *
 * Sample usage:
 *
 * instruction_t ins = ins("nor", reg(REG_R1), imm(44), reg(REG_R2));
 *
 * The above generates an instruction_t equivalent to the instruction:
 * nor %r1, 44, %r2
 *
 * This instruction can then be passed to the emit() function passed as a 
 * pointer to each plugin to cause the assembler to assembler the specified
 * nor instruction.
 *
 * See the sample plugins for more examples of using this function.
 *
 ******************************************************************************/
instruction_t ins(char* mnemonic, int operand_count, ...);
        
#endif
