/*******************************************************************************
 *
 * File         : assember_provided.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Provided types and functions to use in the Western 2208 assembler.
 *
 ******************************************************************************/

#include "hash_table.h"

#ifndef ASSEMBLER_PROVIDED_H
#define ASSEMBLER_PROVIDED_H

/*******************************************************************************
 * ENUMERATIONS
 ******************************************************************************/

// The type of an operand in an assembler instruction or directive
typedef enum {
    OP_REGISTER = 0,	// The operand represents a register
    OP_IMMEDIATE,	// The operand represents a signed, immediate value
    OP_LABEL,		// The operand represents a label
    OP_STRING		// The operand represents a string
} operand_type_t;

// The type of directive encountered by the parser
typedef enum {
    DIR_TEXT = 0,	// .text  (0 operands)
    DIR_DATA,		// .data  (0 operands)
    DIR_BYTE,		// .byte  (1 operand of type OP_IMMEDIATE)
    DIR_HALF,		// .half  (1 operand of type OP_IMMEDIATE)
    DIR_WORD,		// .word  (1 operand of type OP_IMMEDIATE)
    DIR_STR,		// .str   (1 operand of type OP_STRING)
    DIR_ALIGN		// .align (1 operand of type OP_IMMEDIATE)
} directive_type_t;

/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

// An opcode is simply an integer
typedef int opcode_t;

// A segment is simply an integer
typedef int segment_t;

/*******************************************************************************
 * UNIONS
 ******************************************************************************/

// Stores the value of a given operand.  REMEMBER: with a union, you must only
// access the member (string or integer) appropriate for the operand type.
// See operand_t below for more details.
typedef union {

    char* string;	// Interpret the memory of the union as a char*
    int integer;	// Interpret the memory of the union as an integer
    
} operand_value_t;

/*******************************************************************************
 * Structures
 ******************************************************************************/

/****************************************************************
 *
 * Represents an opcode. 
 *
 * Members:
 *   mnemonic	The mnemonic for the instruction (e.g. "nor")
 *   opcode	The opcode for the instruction (e.g. 1)
 *
 ***************************************************************/
typedef struct {
	char*	 mnemonic;	
	opcode_t opcode;
} opcode_info_t;
	
/****************************************************************
 *
 * Represents a single operand for an assembler instruction or 
 * directive.
 *
 * Members:
 *   type	The operand type (e.g. OP_REGISTER)
 *   value	The value of the operand (see note below)
 *
 * Note: the 'value' member is a union.  You must check the 'type'
 *       member of the operand first, and then access the 
 *       appropriate member of the 'value' union.
 *
 * Example:
 *
 * if (op.type == OP_REGISTER)
 *   printf("Register %d", op.value.integer);
 * else if (op.type == OP_LABEL)
 *   printf("Label %s", op.value.string);
 * .
 * .
 * .
 *
 ***************************************************************/
typedef struct {

    operand_type_t type;
    operand_value_t value;
    
} operand_t;

/****************************************************************
 *
 * Represents an assembler instruction
 *
 * Members:
 *   mnemonic		The instruction mnemonic (e.g. "nor")
 *   operands		An array of (up to) 3 operands for the instruction
 *   operand_count	The number of operands actually stored in the array
 *
 * Example:
 *
 * Suppose we wanted to create the instruction "nor %r0, 0xFF, %r1" in code:
 *
 * instruction_t ins;
 * ins.mnemonic = "nor";
 * ins.operand_count = 3;
 * ins.operands[0].type = OP_REGISTER;
 * ins.operands[0].value.integer = REG_R0;  (register macros are defined in western2208.h)
 * ins.operands[1].type = OP_IMMEDIATE;
 * ins.operands[1].value.integer = 0xFF;
 * ins.operands[2].type = OP_REGISTER;
 * ins.operands[2].value.integer = REG_R1   (register macros are defined in western2208.h)
 *
 * Note that this example is just to show you how an instruction_t structure 
 * works.  You will never have to create an instruction manually like this.
 * In your assembler code, the parser will pass instruction_t structures to you.
 * In your plugin code, you've got the ins() function to help you build 
 * instructions quickly and easily.
 *
 ***************************************************************/
typedef struct {

    char* mnemonic;
    operand_t operands[3];
    unsigned char operand_count;

} instruction_t;

/****************************************************************
 *
 * Represents an assembler directive
 *
 * Members:
 *   type		The directive type (e.g. DIR_TEXT, DIR_ALIGN, etc.)
 *   operand		The operand for the directive, if any
 *   has_operand	TRUE, if the directive has an operand; FALSE otherwise
 *
 * Example:
 *
 * This works much like instruction_t.  Suppose we wanted to represent
 * the directive ".align 4":
 *
 * directive_t dir;
 * dir.type = DIR_ALIGN;
 * dir.has_operand = TRUE;
 * dir.operand.type = OP_IMMEDIATE;
 * dir.operand.value.integer = 4;
 *
 * Note that this example is just to show you how an directive_t structure 
 * works.  You will never have to create an directive manually like this.
 * In your assembler code, the parser will pass directive_t structures to you.
 *
 ***************************************************************/
typedef struct {
	directive_type_t type;
	operand_t operand;
	bool_t has_operand;
} directive_t;

/****************************************************************
 *
 * Represents an entry in the user symbol table
 *
 * Members:
 *   label		The name of the label
 *   segment		The segment in which the label appears (DIR_TEXT or DIR_DATA)
 *   address		The value of the location counter in 'segment' where 'label' was found
 *
 * Example:
 *
 * Suppose we wanted to put the label "_hello" -- which appears in the data
 * segment at location 4 -- into the user symbol table:
 *
 * user_symtab_entry_t* entry = (malloc enough memory for the entry)
 * entry->label = "hello";
 * entry->segment = DIR_DATA;
 * entry->address = 4;
 * // now call hash_table_set() and add the entry to the table (see hash_table.h)
 *
 * Remember that we use '->' and not '.' when accessing members on
 * a pointer to a structure
 *
 ***************************************************************/
typedef struct
{
	char* label;
	segment_t segment;
	int address;
	
} user_symtab_entry_t;

/****************************************************************
 *
 * Represents an entry in the permanent symbol table.  THIS IS
 * INITIALIZED FOR YOU BY load_plugin(), so you don't really have
 * to worry about what this structure contains.
 *
 ***************************************************************/
typedef struct
{
	void* plugin_handle;
	char* mnemonic;
	opcode_t opcode;
	bool_t is_synthetic;
	
	union {
		void (*synth_fn)(int, instruction_t, void (*emit)(int, instruction_t));
		void (*fn)(int, instruction_t);
	} handler;
	
} perm_symtab_entry_t;
	
// Used by the parser -- ignore this
typedef struct {
	char* label;
	bool_t is_instruction;
	bool_t is_directive;
	instruction_t instruction;
	directive_t directive; 
} statement_t;

// Used for command line arguments -- ignore this
typedef struct {
	char* input_file;
	bool_t debug_flag;
} assembler_args_t;

/*******************************************************************************
 * FUNCTION DECLARATIONS
 ******************************************************************************/

/*******************************************************************************
 *
 * print_user_symtab
 * 
 * Prints the labels and addresses stored in the user symbol table.  This 
 * function is called automatically when running the assembler in --debug mode
 *
 * Parameters:
 *   user_symtab	The table to print
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void print_user_symtab(hash_table_t* user_symtab);

/*******************************************************************************
 *
 * debug_header
 * 
 * Prints a header and a border in --debug mode
 *
 * Parameters:
 *   step	The header text to print
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void debug_header(char* step);

/*******************************************************************************
 *
 * handle_instruction
 * 
 * Called by the parser each time an instruction is found in the assembly source
 * code.  This function then looks up the instruction in the permanent symbol
 * table and calls its handler function (which will either be the 
 * handle_ns_instruction() in assembler.c for non-synthetic instructions, or
 * your handle() method in one of your plugins for synthetic instructions).  
 * Behold the power of function pointers!
 *
 * Parameters:
 *   pass	The current assembler pass (1 or 2)
 *   stmt	The instruction encountered by the parser
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void handle_instruction(int pass, instruction_t stmt);

#endif

#include "parser.h"
#include "parser.tab.h"	

