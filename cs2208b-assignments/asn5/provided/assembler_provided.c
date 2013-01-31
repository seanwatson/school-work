/*******************************************************************************
 *
 * File         : assembler_provided.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * See function descriptions in assembler_provided.h.
 * 
 ******************************************************************************/

#include "args.h"
#include "assembler.h"
#include "assembler_provided.h"
#include "parser.h"
#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

hash_table_t* user_symtab;
hash_table_t* perm_symtab;
bool_t debug_mode = FALSE;
segment_t current_segment = DIR_TEXT;
int text_loc = 0;
int data_loc = 0;
int current_loc = 0;
FILE* output;

static int user_symtab_sorter(const void* a, const void* b)
{
	return (*(const user_symtab_entry_t**)a)->address - (*(const user_symtab_entry_t**)b)->address;
}

void print_user_symtab(hash_table_t* user_symtab)
{
	user_symtab_entry_t** entries = (user_symtab_entry_t**)hash_table_values(user_symtab);
	user_symtab_entry_t** ds_entries = NULL;
	user_symtab_entry_t** ts_entries = NULL;
	int ds_count = 0;
	int ts_count = 0;
	int i;
	
	for (i = 0; i < user_symtab->size; ++i)
	{
		if (entries[i]->segment == DIR_DATA)
		{
			ds_entries = (user_symtab_entry_t**)realloc(ds_entries, (ds_count + 1) * sizeof(user_symtab_entry_t*));
			ds_entries[ds_count++] = entries[i];
		}
		else
		{
			ts_entries = (user_symtab_entry_t**)realloc(ts_entries, (ts_count + 1) * sizeof(user_symtab_entry_t*));
			ts_entries[ts_count++] = entries[i];
		}
	}
			
	if (ds_count > 0)
		qsort(ds_entries, ds_count, sizeof(user_symtab_entry_t*), user_symtab_sorter);
				
	if (ts_count > 0)
	 	qsort(ts_entries, ts_count, sizeof(user_symtab_entry_t*), user_symtab_sorter);
		
	printf("Segment         Label            Address\n");
	printf("========================================\n");
	
	for (i = 0; i < ds_count; ++i)
	{
		printf("DATA            %-16s %7d\n", ds_entries[i]->label, ds_entries[i]->address);
	}
	
	for (i = 0; i < ts_count; ++i)
	{
		printf("TEXT            %-16s %7d\n", ts_entries[i]->label, ts_entries[i]->address);
	}
	
	if (ds_count + ts_count == 0)
		printf("No entries in the user symbol table\n");
		
	if (ds_count > 0)
		free(ds_entries);
		
	if (ts_count > 0)
		free(ts_entries);
		
	if (entries)
		free(entries);
}

void debug_header(char* step)
{
	printf("%s\n", step);
	printf("--------------------------------------------------------------------------------\n");
}

void handle_instruction(int pass, instruction_t i) {
	
	perm_symtab_entry_t* entry = hash_table_get(perm_symtab, i.mnemonic);

	if (entry != NULL)
	{
		//printf("Entry opcode: %d\n", entry->opcode);
		if (entry->is_synthetic)
			entry->handler.synth_fn(pass, i, handle_instruction);
		else {
			entry->handler.fn(pass, i);
		}
	}	
	else
		yyerror(pass, "Unknown instruction found");


//	entry = hash_table_get(perm_symtab,"stw");
//	printf("Opcode: %d\n", entry->opcode);
}

void build_perm_symtab()
{
	int i;
	perm_symtab_entry_t* entry;
	perm_symtab = hash_table_create();
	
	char* mnemonics[] = {
		
		"nor",  "add", "tst",  "srl", "sra", "sll", "ldb", 
		"ldub", "ldh", "lduh", "ldw", "stb", "sth", "stw"
	};
	
	for (i = 0; i < OPCODE_COUNT; ++i)
	{
		entry = (perm_symtab_entry_t*)malloc(sizeof(perm_symtab_entry_t));
		entry->mnemonic = mnemonics[i];
//		if(i == 10){
//			entry->opcode = 14;
//		}
//		else if (i == 13){
//			entry->opcode = 11;
//		}
//		else{
			entry->opcode = i + 1;
//		}
		entry->is_synthetic = FALSE;
		entry->handler.fn = handle_ns_instruction;
		
		hash_table_set(perm_symtab, entry->mnemonic, entry);
	}
}

void initialize_tables()
{
	user_symtab = hash_table_create();
	build_perm_symtab();
	load_plugins();
}

int assemble(assembler_args_t args)
{
	// Open the input file
	FILE* input = fopen(args.input_file, "r");  
	char* output_filename = strdup(basename(args.input_file));
	output_filename[strrchr(output_filename, '.')-output_filename] = '\0';
	// If we couldn't open it, print an error and exit
        if (!input)
        {
                perror(args.input_file);
                return EXIT_FAILURE;
        }

	// Check if we ran the program with --debug
	if (args.debug_flag)
	{
		debug_mode = TRUE;
		debug_header("Plugins");
	}
	
	// Initialize the user and permanent symbol tables		
	initialize_tables();
		
	// Perform the first pass over the input
        yyrestart(input);
        yylineno = 1;
        yyparse(FIRST_PASS);

	// Rewind the input file in preparation for the second pass
	rewind(input);
	
	// Store the current location counter
	if (current_segment == DIR_TEXT)
		text_loc = current_loc;
	else
		data_loc = current_loc;

	// Open the output file and write the EF2208 header
	
	output = fopen(output_filename,"wb");
	free(output_filename);
	write_header();
	
	if (debug_mode) {
		
		debug_header("\nHeader Information");
		printf("Size of data segment: %d\n", data_loc);
		printf("Size of text segment: %d\n\n", text_loc);

		debug_header("Symbol Table");		
		printf("\nBefore relocation:\n\n");
		print_user_symtab(user_symtab);
	}
	
	// Relocate labels in the text segment so they appear after the 
	// data segment
	relocate_text_segment();

	if (debug_mode)
	{
		printf("\nAfter relocation:\n\n");
		print_user_symtab(user_symtab);
	}
	
	// Reset the location counters and change back to the text segment
	data_loc = 0;
	text_loc = 0;
	current_loc = 0;
	current_segment = DIR_TEXT;
	
	// Perform the second pass over the input
	yyrestart(input);
	yylineno = 1;
	yyparse(SECOND_PASS);

	// Close the input and output files
        fclose(input);
	fclose(output);

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	assembler_args_t args = parse_args(argc, argv);
	return assemble(args);
}

