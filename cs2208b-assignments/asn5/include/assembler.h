#include "boolean.h"
#include "western2208.h"
#include "loader.h"
#include "assembler_provided.h"

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/*******************************************************************************
 *
 * handle_directive
 * 
 * Called by the parser each time a directive is found in the assembly source
 * code.  This function then takes the appropriate action depending on the 
 * current assembler pass (1 or 2) and/or the given directive.
 *
 * Parameters:
 *   pass	The current assembler pass (1 or 2)
 *   stmt	The assembler directive encountered by the parser
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void handle_directive(int pass, directive_t stmt);


/*******************************************************************************
 *
 * handle_label
 * 
 * Called by the parser each time a label is found in the assembly source
 * code.  In pass 1, this function adds the label and the current location 
 * counter to the user symbol table.  In pass 2, this function simply ignores
 * the label since the user symbol table has already been built.
 *
 * Parameters:
 *   pass	The current assembler pass (1 or 2)
 *   label	The label encountered by the parser
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void handle_label(int pass, char* label);


/*******************************************************************************
 *
 * handle_ns_instruction  (handle non-synthetic instruction)
 * 
 * Each time the parser encounters an instruction, it calls handle_instruction()
 * in assembler_provided.h.  This function looks up the instruction in the 
 * permanent symbol table and calls its handler function.
 *
 * If the instruction is synthetic (i.e. it is provided by a plugin and is not
 * an instruction native to the Western 2208 CPU, such as nor, tst, etc.), then
 * the handler function in the plugin is called.
 *
 * If the instruction is non-synthetic (i.e. it is a native instruction in the
 * Western 2208 CPU), then this function is called.
 *
 * This function will then increment the location counter appropriately, and,
 * if we're in pass 2, will encode the instruction in the Western 2208 machine
 * language format and output it to the executable.
 *
 * Parameters:
 *   pass	The current assembler pass (1 or 2)
 *   i		The instruction encountered by the parser
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void handle_ns_instruction(int pass, instruction_t i);


/*******************************************************************************
 *
 * write_header
 * 
 * Called before pass 2 starts to write the 2208EF (2208 Executable Format) 
 * header to the executable file.  This header consists of:
 *
 * - A 6-byte 'magic number':  2208EF
 * - A 4-byte integer containing the size of the data segment
 * - A 4-byte integer containing the size of the text segment
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void write_header();


/*******************************************************************************
 *
 * relocate_text_segment
 * 
 *  This function is called after the first pass.  We have two location
 * counters -- one for the data segment and one for the text segment.
 * Both counters start at 0.  However, in pass 2, we are going to be
 * writing to the executable file, so we need to relocate the text
 * segment so that it comes after the data segment in the executable
 * file.
 * 
 * In our case, we simply need to adjust the addresses of all labels
 * in the text segment so that they have unique addresses occuring 
 * after the end of the data segment.
 * 
 * So, you'll iterate through all entries in the user symbol table.  If
 * you find any labels in the text segment, you'll need to relocate them
 * (change their addresses) so that they are relative to the data 
 * segment.
 * 
 * For example, suppose we have the following code:
 * 
 * .data
 * _i: .word 0xAABBCCDD       # Bytes 0 - 3
 * _j: .half 0xAABB           # Bytes 4 - 5
 * _k: .half 0xCCDD           # Bytes 6 - 7
 * 
 * .text
 * _main: mov 0, %r1          # Bytes 8 - 11
 *        mov 1, %r2          # Bytes 12 - 15
 * _loop: ...                 # Bytes 16 - ...
 * 
 * Before relocation, our symbol table would look like the following
 * (remember: there are two separate location counters):
 * 
 * Before relocation:
 * 
 * Segment         Label            Address
 * ========================================
 * DATA            _i                     0
 * DATA            _j                     4
 * DATA            _k                     6
 * TEXT            _main                  0
 * TEXT            _loop                  8
 * 
 * After relocation:
 * 
 * Segment         Label            Address
 * ========================================
 * DATA            _i                     0
 * DATA            _j                     4
 * DATA            _k                     6
 * TEXT            _main                  8
 * TEXT            _loop                 16  
 * 
 * Hint: how could we use the data location counter to accomplish this
 *       task?
 * 
 * To get an array of values in the user_symbol_table, see the 
 * description of the hash_table_values() function in hash_table.h.
 * Note that this function dynamically allocates memory for the array
 * it returns.  Thus, you will need to free this memory at the end of
 * this function when you are done with the array.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void relocate_text_segment();

#endif
