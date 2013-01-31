/*******************************************************************************
 *
 * File         : cpu.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Main program for the Western 2208 CPU simulator and debugger.  You won't 
 * need to work with this file.
 *
 ******************************************************************************/

#include <signal.h>
#include "boolean.h"

#ifndef CPU_H
#define CPU_H

typedef struct
{
	char* filename;
	bool_t debug_flag;
	bool_t disasm_flag;
} cpu_args_t;

typedef enum
{
	INS_NOP = 0,
	INS_NOR,
	INS_ADD,
	INS_TST,
	INS_SRL,
	INS_SRA,
	INS_SLL,
	INS_LDB,
	INS_LDUB,
	INS_LDH,
	INS_LDUH,
	INS_LDW,
	INS_STB,
	INS_STH,
	INS_STW
} ml_opcode_t;
	
#endif
