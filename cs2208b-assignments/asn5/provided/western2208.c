/*******************************************************************************
 *
 * File         : western2208.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : April 7, 2012
 * 
 * See function descriptions in western2208.h.
 *
 ******************************************************************************/

#include "western2208.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

// Forward declarations
static void nop(cpu_t*, ml_operands_t*);
static void nor(cpu_t*, ml_operands_t*);
static void add(cpu_t*, ml_operands_t*);
static void tst(cpu_t*, ml_operands_t*);
static void srl(cpu_t*, ml_operands_t*);
static void sra(cpu_t*, ml_operands_t*);
static void sll(cpu_t*, ml_operands_t*);
static void ldb(cpu_t*, ml_operands_t*);
static void ldub(cpu_t*, ml_operands_t*);
static void ldh(cpu_t*, ml_operands_t*);
static void lduh(cpu_t*, ml_operands_t*);
static void ldw(cpu_t*, ml_operands_t*);
static void stb(cpu_t*, ml_operands_t*);
static void sth(cpu_t*, ml_operands_t*);
static void stw(cpu_t*, ml_operands_t*);
static void unimpl(cpu_t*, ml_operands_t*);

static void (*execute[])(cpu_t*, ml_operands_t*) = {
	nop,	// 0
	nor,	// 1
	add,	// 2
	tst,    // 3
	srl,	// 4
	sra,	// 5
	sll,	// 6
	ldb,	// 7
	ldub,	// 8
	ldh,	// 9
	lduh,	// 10
	ldw,	// 11
	stb,	// 12
	sth,	// 13
	stw,	// 14
	unimpl, // 15
	unimpl, // 16
	unimpl, // 17
	unimpl, // 18
	unimpl, // 19
	unimpl, // 20
	unimpl, // 21
	unimpl, // 22
	unimpl, // 23
	unimpl, // 24
	unimpl, // 25
	unimpl, // 26
	unimpl, // 27
	unimpl, // 28
	unimpl, // 29
	unimpl, // 30
	unimpl  // 31																	
};

static void nop(cpu_t* cpu, ml_operands_t* operands)
{
	;
}

static void nor(cpu_t* cpu, ml_operands_t* operands) 
{ 
	REG(operands->rd) = ~(REG(operands->rs1) | operands->op2);
}


static void add(cpu_t* cpu, ml_operands_t* operands)
{
	uint64_t sum_unsigned = (uint64_t)REG(operands->rs1) + (uint64_t)operands->op2;
	int32_t sum_signed = (int32_t)REG(operands->rs1) + (int32_t)operands->op2;
	uint32_t sign_op1 = REG(operands->rs1) & 0x80000000;
	uint32_t sign_op2 = operands->op2 & 0x80000000;
	uint32_t sign_res = sum_signed & 0x80000000;

	REG(REG_FC) = REG(REG_FV) = 0x0;
	
	if (sum_unsigned > UINT32_MAX)
	{
		REG(REG_FC) = 0xFFFFFFFF;
	}

	if ((sign_op1 == sign_op2) && (sign_op1 != sign_res))
		REG(REG_FV) = 0xFFFFFFFF;
		
	REG(operands->rd) = (uint32_t)sum_unsigned;
}

static void tst(cpu_t* cpu, ml_operands_t* operands)
{
	REG(REG_FN) = REG(REG_FZ) = 0x0;
		
	if ((int32_t)REG(operands->rs1) < 0)
	{
		REG(REG_FN) = 0xFFFFFFFF;
	}
	else if (REG(operands->rs1) == 0)
	{
		REG(REG_FZ) = 0xFFFFFFFF;			
	}
}

static void srl(cpu_t* cpu, ml_operands_t* operands)
{
	REG(operands->rd) = (((uint32_t)REG(operands->rs1)) >> (operands->op2 & 0x1F));
}

static void sra(cpu_t* cpu, ml_operands_t* operands)
{
	REG(operands->rd) = (uint32_t)(((int32_t)REG(operands->rs1)) >> (operands->op2 & 0x1F));
}

static void sll(cpu_t* cpu, ml_operands_t* operands)
{
	REG(operands->rd) = REG(operands->rs1) << (operands->op2 & 0x1F);
}

static void ldb(cpu_t* cpu, ml_operands_t* operands)
{
	REG(operands->rd) = cpu->process->ram.bytes[REG(operands->rs1) + operands->op2];
}

static void ldub(cpu_t* cpu, ml_operands_t* operands)
{
	REG(operands->rd) = cpu->process->ram.bytes[REG(operands->rs1) + operands->op2] & 0xFF;
}

static void ldh(cpu_t* cpu, ml_operands_t* operands)
{
	uint32_t addr = REG(operands->rs1) + operands->op2;
	
	if (addr % 2 != 0)
		raise(SIGBUS);
		
	REG(operands->rd) = cpu->process->ram.halfwords[addr / 2];	
}

static void lduh(cpu_t* cpu, ml_operands_t* operands)
{
	uint32_t addr = REG(operands->rs1) + operands->op2;
	
	if (addr % 2 != 0)
		raise(SIGBUS);
			
	REG(operands->rd) = cpu->process->ram.halfwords[addr / 2] & 0xFF;		
}

static void ldw(cpu_t* cpu, ml_operands_t* operands)
{
	uint32_t addr = REG(operands->rs1) + operands->op2;
	
	if (addr % 4 != 0)
		raise(SIGBUS);
			
	REG(operands->rd) = cpu->process->ram.words[addr / 4];		
}

static void stb(cpu_t* cpu, ml_operands_t* operands)
{
	cpu->process->ram.bytes[REG(operands->rd) + operands->op2] = REG(operands->rs1);
}

static void sth(cpu_t* cpu, ml_operands_t* operands)
{
	uint32_t addr = REG(operands->rd) + operands->op2;
	
	if (addr % 2 != 0)
		raise(SIGBUS);
			
	cpu->process->ram.halfwords[addr / 2] = REG(operands->rs1);
}

static void stw(cpu_t* cpu, ml_operands_t* operands)
{
	uint32_t addr = REG(operands->rd) + operands->op2;

	if (addr % 4 != 0)
		raise(SIGBUS);
			
	cpu->process->ram.words[addr / 4] = REG(operands->rs1);
}

static void unimpl(cpu_t* cpu, ml_operands_t* operands)
{
	raise(SIGILL);
}

static void flush_input_buffer()
{
	char ch;
	
	while ((ch = getchar()) != '\n' && ch != EOF)
	{
		;
	}
}

static void handle_io(cpu_t* cpu)
{
	uint8_t c;
	int32_t i;
	char ch;
	
	switch(REG(REG_IO))
	{
		case REQ_READ_CHAR:
			REG(REG_R1) = getchar();
			REG(REG_IO) = 0x0;
			flush_input_buffer();
			break;
		
		case REQ_READ_INT:
			scanf("%d",&i);
			REG(REG_R1) = i;
			REG(REG_IO) = 0x0;
			flush_input_buffer();
			break;
		
		case REQ_PRINT_CHAR:
			printf("%c", REG(REG_R1));
			REG(REG_IO) = 0x0;
			break;
		
		case REQ_PRINT_INT:
			printf("%d", REG(REG_R1));
			REG(REG_IO) = 0x0;
			break;
		
		case REQ_PRINT_STR:
			printf("%s", cpu->process->ram.bytes + REG(REG_R1));
			REG(REG_IO) = 0x0;
			break;	
	}
	

}

ml_operands_t fetch(cpu_t* cpu)
{
	ml_operands_t operands;
	
	if ((REG(REG_PC) % 4 != 0) || (cpu->process->size - REG(REG_PC) < 4))
		raise(SIGBUS);
		
	REG(REG_IR) = cpu->process->ram.words[REG(REG_PC) / 4];
	
	operands.rs1 = RS1(REG(REG_IR));
	operands.op2 = (int32_t)RS2_OR_IMM(REG(REG_IR));
	operands.rd = RD(REG(REG_IR));
	operands.imm_flag = IMM(REG(REG_IR));

	if (! operands.imm_flag)
		operands.op2 = REG(operands.op2);
		
	return operands;
}

cpu_t* schedule(process_t* process)
{
	cpu_t* cpu = calloc(1, sizeof(cpu_t));
	cpu->process = process;
	REG(REG_PC) = process->entry_point;
	
	return cpu;
}

void run(cpu_t* cpu)
{
	int i;
	ml_operands_t operands;
	uint16_t last_pc;

	while (REG(REG_PC) < cpu->process->size)
	{
		REG(REG_R0) = 0;
		operands = fetch(cpu);
		raise(SIGUSR1);
		last_pc = REG(REG_PC);
		execute[OP(REG(REG_IR))](cpu, &operands);
				
		if ((REG(REG_PC) == 0xFFFFFFFF) || (REG(REG_PC) == last_pc))
		 	REG(REG_PC) = last_pc + 4;
		
		handle_io(cpu);
	}
}
