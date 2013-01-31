/*******************************************************************************
 * 
 * File:	 assembler.c
 * Contributors: Sean Watson, Jeff Shantz
 * Last Updated: April 8, 2012
 *
 * Functions used by the Western2208 assembler
 *
 *******************************************************************************/

#include "assembler.h"
#include "assembler_provided.h"
#include "hash_table.h"
#include "plugins.h"
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANTS
 *******************************************************************************/

#define PLUGIN_DIR "./plugins/"		//directory to look in for plugins
#define BYTE 1				//length of a byte in bytes
#define HALF 2				//length of a half word in bytes
#define WORD 4				//length of a word in bytes
#define INSTR_LEN 4			//length of a western2208 instruction in bytes
#define FIVE_BITS 0x1F			//highest five bit value
#define SIXTEEN_BITS 0xFFFF		//highest sixteen bit value
#define	FALSE 0				//false value
#define TRUE 1				//true value
#define IMM_TRUE 0x00010000		//for setting the imm bit true
#define IMM_FALSE 0xFFFEFFFF		//for setting the imm bit false

/*******************************************************************************
 * VARIABLES
 *******************************************************************************/

extern hash_table_t* user_symtab;	// User symbol table
extern hash_table_t* perm_symtab;	// Permanent symbol table
extern bool_t debug_mode;		// Whether or not we're in debug mode
extern segment_t current_segment;	// The current segment (DIR_TEXT or DIR_DATA)
extern int text_loc;			// Text segment location counter
extern int data_loc;			// Data segment location counter
extern int current_loc;			// Current location counter
extern FILE* output;			// Output file (the executable file)


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
void write_header()
{
	//get the magic number
	char magic_number[] = MAGIC_NUMBER;

	//write the magic number to the output file, show an error if unsuccessful
	if (fwrite(magic_number, BYTE, strlen(magic_number), output) != strlen(magic_number)){
		fprintf(stderr, "Failure writing magic number to output");
		exit(EXIT_FAILURE);
	}

	//write the size of the data segment to the output file, show an error if unsuccessful
	if (fwrite(&data_loc, BYTE, sizeof(int), output) != sizeof(int)){
		fprintf(stderr, "Failure writing data segment size to output");
		exit(EXIT_FAILURE);
	}

	//write the size of the text segment to the output file, show an error if unsuccessful
	if (fwrite(&text_loc, BYTE, sizeof(int), output) != sizeof(int)){
		fprintf(stderr, "Failure writing text segment size to output");
		exit(EXIT_FAILURE);
	}
}

/*******************************************************************************
 *
 * write_instruction
 *
 * Combines the given arguments into a 32-bit Western2208 machine language
 * instruction and writes it to the output file
 *
 * Parameters:
 *    opcode	 The 5 bit integer value of the opcode
 *    rs1	 The first source register (5 bits)
 *    imm	 Whether the second source is a register or immediate value
 *    rs2_or imm The second source register (5 bits + paddint) or a 16-bit imm
 *    rd	 The destination register (5 bits)
 *
 * Returns:
 *    Nothing
 *
 *******************************************************************************/
void write_instruction(opcode_t opcode, int rs1, bool_t imm, int rs2_or_imm, int rd)
{
	//begin with an empty instruction
	int instruction = 0;

	//add the opcode to the instruction
	instruction |= (opcode << 27);

	//make sure the destination register is valid
	if(rd > FIVE_BITS){
		fprintf(stderr, "Invalid destination register");
		exit(EXIT_FAILURE);
	}

	//add the destination register to the instruction
	instruction |= (rd << 22);

	//make sure the source register is valid
	if(rs1 > FIVE_BITS){
		fprintf(stderr, "Invalid source register");
		exit(EXIT_FAILURE);
	}

	//add the first source register to the instruction
	instruction |= (rs1 << 17);

	//add the proper imm value to the instruction
	if (imm){
		instruction |= IMM_TRUE;
	}
	else{
		instruction &= IMM_FALSE;
	}

	//add the second source or immediate value to the instruction
	instruction |= rs2_or_imm;

	//write the instruction, print an error if unsuccessful
	if(fwrite(&instruction, BYTE, INSTR_LEN, output) != INSTR_LEN){
		fprintf(stderr, "Failure writing instruction to output file");
		exit(EXIT_FAILURE);
	}
}

/*******************************************************************************
 *
 * relocate_text_segment
 *
 * Called after the first pass to offset the addresses of the
 * text segment labels so that they come after the data segment. 
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void relocate_text_segment()
{

	//start the offset at the end of the data segment
	int offset = data_loc;

	//align the offset to a 4 byte boundary
	while (offset % WORD){
		offset++;
	}

	//get all of the labels in the user table
	user_symtab_entry_t** labels = (user_symtab_entry_t**) hash_table_values(user_symtab);
	
	//add the offset to all labels in the text segment
	int i;
	for(i=0; i < user_symtab->size; i++){
		if(labels[i]->segment == DIR_TEXT){
			labels[i]->address += offset;
		}
	}

	//release the memory used when we got the labels
	free(labels);

}

/*******************************************************************************
 *
 * get_rs2_or_imm
 * 
 * Takes the operand passed to the function (which should be the second operand
 * in a three operand instruction) and returns the appropriate integer value.
 * If it is a register it returns the register number. If it is an immediate
 * value it returns the immediate value. If it is a label it returns the
 * address
 *
 * Parameters:
 *   operand	The operand to get the value from
 *
 * Returns:
 *   An integer value corresponding to the register number, immediate value,
 *   or label address of the operand depending on its type
 *
 ******************************************************************************/
int get_rs2_or_imm(operand_t operand)
{
	//create a pointer for the label entry if it is a label operand
	user_symtab_entry_t* label;
	
	//return the integer value if it is a register or immediate value
	if (operand.type == OP_IMMEDIATE){
		return operand.value.integer;
	}
	else if(operand.type == OP_REGISTER && operand.value.integer <= FIVE_BITS){
		return operand.value.integer;
	}
	else{
		//get the entry corresponding to the label, print an error if it doesnt exist
		if(label = hash_table_get(user_symtab, operand.value.string)){

			//return the address corresponding to the label
			return label->address;
		}
		else{
			fprintf(stderr, "Unknown label '%s'", operand.value.string);
			exit(EXIT_FAILURE);
		}
	}
}

/******************************************************************************
 *
 * do_align
 *
 * this function aligns the current location counter to the
 * specified boundary. In pass one it just aligns while in pass
 * two it also writes 0s to the empty bytes in the output file
 *
 * Parameters:
 *    pass	The current assembler pass (1 or 2)
 *    align	The boundary to align to (ex 4 would align to a location 
 *              divisible by 4)
 *
 * Returns:
 *    Nothing
 *
 *****************************************************************************/
void do_align(int pass, int align)
{
	//continue until the boundary is reached	
	while(current_loc % align){

		//increment the location counter
		current_loc++;

		//if it is pass two, write a 0 to the output file
		if(pass == 2){
			fputc(0, output);
		}
	}
}

/*******************************************************************************
 *
 * handle_ns_instruction  (handle non-synthetic instruction)
 * 
 * If the instruction is non-synthetic (i.e. it is a native instruction in the
 * Western 2208 CPU) this function is called.
 *
 * This function will increment the location counter appropriately, and,
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
void handle_ns_instruction(int pass, instruction_t i)
{

	//increment the location counter by the length of an instrunction
	current_loc += INSTR_LEN;
	
	//if were in pass one do nothing else
	if (pass == 1){
		return;
	}

	//get the instruction corresponding to the mnemonic
	perm_symtab_entry_t* instruction = hash_table_get(perm_symtab, i.mnemonic);

	//if the mnemonic doesn't exist print an error message and quit
	if (!instruction){
		fprintf(stderr, "Invalid instruction '%s'", i.mnemonic);
		exit(EXIT_FAILURE);
	}
	
	//create the parts of the instruction
	opcode_t opcode = instruction->opcode;
	int rs1;
	int rs2_or_imm;
	int rd;
	bool_t imm;
	
        switch(i.operand_count)
        {
                case 1:
			//make sure the one operand is a register
			if(i.operands[0].type != OP_REGISTER){
				fprintf(stderr, "Invalid operand type\n");
				exit(EXIT_FAILURE);
			}
			
			//set the source register as the first operand
                        rs1 = i.operands[0].value.integer;

			//set all other parameters to 0
			rs2_or_imm = 0;
			rd = 0;
			imm = 0;
			break;
                case 2:
			//make sure both operands are registers
			if(i.operands[0].type != OP_REGISTER || i.operands[1].type != OP_REGISTER){
				fprintf(stderr, "Invalid operand type\n");
				exit(EXIT_FAILURE);
			}
			//set the first source and destination register
			rs1 = i.operands[0].value.integer;
			rd = i.operands[1].value.integer;
			
			//set the rest of the parameters to 0
			rs2_or_imm = 0;
			imm = 0;
                        break;
                case 3:
			//make sure the first source and destination are registers, the second source isn't a string
			if(i.operands[0].type != OP_REGISTER || i.operands[1].type == OP_STRING || i.operands[2].type != OP_REGISTER){
				fprintf(stderr, "Invalid operand type\n");
				exit(EXIT_FAILURE);
			}

			//set the first source register
			rs1 = i.operands[0].value.integer;

			//set the second source register using the helper function
			rs2_or_imm = get_rs2_or_imm(i.operands[1]);

			//set the destination register
			rd = i.operands[2].value.integer;

			//set the imm value appropriately
			if(i.operands[1].type == OP_REGISTER){
				imm = FALSE;
			}
			else{
				//make sure the constant fits into 16 bits
				if(rs2_or_imm > SIXTEEN_BITS){
					fprintf(stderr, "Immediate value too large\n");
					exit(EXIT_FAILURE);
				}
				imm = TRUE;
			}
                        break;
        }

	//write the instruction to the output file using a helper function
	write_instruction(opcode, rs1, imm, rs2_or_imm, rd);
	
}

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
void handle_directive(int pass, directive_t dir)
{
	int length; //for storing the length of a string directive

	//handle the directive appropriately
	switch(dir.type)
	{
		case DIR_TEXT:
			do_align(pass, WORD);		//align to a 4 byte boundary
			data_loc = current_loc;		//save the data location
			current_loc = text_loc;		//move to the text location
			current_segment = DIR_TEXT;	//set the current segment
			break;
		
		case DIR_DATA:
			text_loc = current_loc;		//save the text location
			current_loc = data_loc;		//move to the data location
			current_segment = DIR_DATA;	//set the current segment
			break;

		case DIR_ALIGN:

			//align to the proper boundary using the helper function
			do_align(pass, dir.operand.value.integer);
			break;
			
		case DIR_BYTE:
			
			//increment the location counter
			current_loc += BYTE;
			
			//if its the first pass do nothing else
			if(pass == 1){
				break;
			}
			
			//get the byte's value
			unsigned char byte = dir.operand.value.integer;
			
			//write it to the output file, print an error if unsuccessful write
			if(fwrite(&byte, BYTE, BYTE, output) != BYTE){
				fprintf(stderr, "Error writing .byte value to output");
				exit(EXIT_FAILURE);
			}
			break;		

		case DIR_HALF:

			//increment the location counter
			current_loc += HALF;
			
			//if it is the first pass do nothing else
			if(pass == 1){
				break;
			}

			//get the half word value
			unsigned short half = dir.operand.value.integer;
			
			//write it to the output file, print an error if unsuccessful
			if(fwrite(&half, BYTE, HALF, output) != HALF){
				fprintf(stderr, "Error writing .half value to output");
				exit(EXIT_FAILURE);
			}
			break;	
			
		case DIR_WORD:

			//increment the location counter
			current_loc += WORD;

			//if it is the first pass do nothing else
			if(pass == 1){
				break;
			}
			
			//write the word to the output value, print an error if unsuccessful
			if(fwrite(&dir.operand.value.integer, BYTE, WORD, output) != WORD){
				fprintf(stderr, "Error writing .word value to output");
				exit(EXIT_FAILURE);
			}
			break;	
			
		case DIR_STR:

			//get the length of the string plus the null character
			length = strlen(dir.operand.value.string) + 1;

			//increment the location counter
			current_loc += length;

			//if it is the first pass do nothing else
			if(pass == 1){
				break;
			}

			//write the string to the output file, print an error if unsuccessful
			if(fwrite(dir.operand.value.string, BYTE, length, output) != length){
				fprintf(stderr, "Error writing .str value to output");
				exit(EXIT_FAILURE);
			}
			break;				
	}
}

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
void handle_label(int pass, char* label)
{
	//do nothing unless this is the first pass
	if(pass != 1){
		return;
	}
	
	// check if the label already exists in the table, print error if it is a duplicate
	if(!hash_table_get(user_symtab, label)){

		// intialize a new table entry
		user_symtab_entry_t* new_label = (user_symtab_entry_t*) malloc(sizeof(user_symtab_entry_t));
		
		// error check the malloc, print error if unsuccessful allocation
		if(!new_label){
			fprintf(stderr,"You need more ram brah");
			exit(EXIT_FAILURE);
		}
		
		// assign the data to the table entry
		new_label->label = label;
		new_label->segment = current_segment;
		new_label->address = current_loc;
		
		// put the entry into the table, print error if error inserting
		if(hash_table_set(user_symtab, label, new_label)){
			fprintf(stderr,"Failure adding label '%s'\n", label);
			exit(EXIT_FAILURE);
		}
	}
	else{
		fprintf(stderr,"Duplicate label: '%s' already exists\n", label);
		exit(EXIT_FAILURE);
	}
}

/*******************************************************************************
 *
 * load_plugins
 *
 * This function is called when the assembler first starts up 
 * (before it begins parsing).  At this point, the permanent symbol
 * table will already have been built, but this function will load
 * any plugins present and store them in the permanent symbol table
 * as well, adding new synthetic instructions to the assembler.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void load_plugins()
{
	
	int count;				//the number of plugins loaded
	char plugin_dir[] = PLUGIN_DIR;		//directory to load plugins from

	//load the list of plugin files
	char** plugin_files = get_plugin_names(plugin_dir, &count);

	perm_symtab_entry_t* current_plugin;	//plugin to add to the table
	
	//iterate through all plugins
	int i;
	for(i=0; i<count; i++){

		// load the plugin, display error if error loading the file
		if(current_plugin = load_plugin(plugin_files[i])){
			
			// add the plugin to the table, display error if error adding to table
			if(!hash_table_set(perm_symtab, current_plugin->mnemonic, current_plugin)){

				// display debug message if debug mode
				if(debug_mode){
					printf("Loaded plugin '%s'\n", current_plugin->mnemonic);
				}
			}
			else{
				fprintf(stderr, "Failure inserting '%s' into permanent symbol table\n", current_plugin->mnemonic);
				exit(EXIT_FAILURE);
			}
		}
		else{
			fprintf(stderr, "Failure loading plugins\n");
			exit(EXIT_FAILURE);
		}
	}
}
