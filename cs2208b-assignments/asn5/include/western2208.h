/*******************************************************************************
 *
 * File         : western2208.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Emulates the Western 2208 microcontroller.  Entering the market at a 
 * $0.15/controller price point, the Western 2208 is positioned to become a
 * disruptive technology.  It offers a cost effective solution for powering
 * consumer electronic devices (e.g. clocks, watches, MP3 players, etc.),
 * providing the performance you need, for the price you could only dream of.
 *
 * Features of the Western 2208 include:
 *
 * - A byte-addressable, load/store RISC architecture
 *
 * - A simple instruction set: only 13 opcodes and only 1 machine language format
 * *
 * - Two's complement representation for signed numbers
 *
 * - 8 general purpose 32-bit registers (%r0 through %r7).  %r0 always stores 0
 *   (just as %g0 always store 0 on the SPARC)
 *
 * - 15 additional 32-bit registers not accessible to the programmer
 *
 * - A 16-bit virtual address.  Hence, the microcontroller is capable of
 *   addressing 64K of RAM (0 to 2^16 - 1)
 *
 * - Capable of performing simple I/O
 *
 * - No cache or pipelining
 *
 ******************************************************************************/

#include <stdint.h>
#include "boolean.h"

#ifndef WESTERN2208_H
#define WESTERN2208_H

#define REG(r) cpu->reg[r]
#define OP(i) (((uint32_t)i) >> 27)
#define RD(i) (uint8_t)((i & 0x7C00000) >> 22)
#define RS1(i) (uint8_t)((i & 0x3E0000) >> 17)
#define IMM(i) ((i & 0x10000) >> 16)     
#define RS2_OR_IMM(i) ((int32_t)(int16_t)i)

#define OPCODE_COUNT 14
#define REG_COUNT 23
#define VIRTUAL_ADDRESS_SIZE 16
#define RAM_SIZE (1 << (VIRTUAL_ADDRESS_SIZE - 1))

typedef enum {
    REQ_READ_CHAR = 0x1,
    REQ_READ_INT  = 0x2,
    REQ_PRINT_CHAR = 0x3,
    REQ_PRINT_INT  = 0x4,
    REQ_PRINT_STR  = 0x5
} io_request_t;

typedef enum {
    REG_R0 = 0,   // 0
    REG_R1,       // 1
    REG_R2,       // 2
    REG_R3,       // 3
    REG_R4,       // 4
    REG_R5,       // 5
    REG_R6,       // 6
    REG_R7,       // 7
    REG_TMP0,     // 8
    REG_TMP1,     // 9
    REG_TMP2,     // 10
    REG_TMP3,     // 11
    REG_TMP4,     // 12
    REG_TMP5,     // 13
    REG_TMP6,     // 14
    REG_TMP7,     // 15
    REG_PC,       // 16
    REG_IR,       // 17
    REG_FZ,       // 18
    REG_FN,       // 19
    REG_FV,       // 20
    REG_FC,       // 21
    REG_IO        // 22
} reg_t;

typedef union
{
		uint8_t bytes[RAM_SIZE];
		uint16_t halfwords[RAM_SIZE / sizeof(short)];
		uint32_t words[RAM_SIZE / sizeof(int)];
} ram_t;
	
typedef struct
{
	int entry_point;
	int size;
	ram_t ram;

} process_t;

typedef struct
{
	process_t* process;
	uint32_t reg[REG_COUNT];
} cpu_t;

typedef struct
{
	uint8_t rs1;
	int32_t op2;
	uint8_t rd;
	bool_t imm_flag;
	
} ml_operands_t;

cpu_t* schedule(process_t* process);
void run(cpu_t* cpu);

#endif
