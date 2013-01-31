/*******************************************************************************
 *
 * File         : loader.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * See function descriptions in loader.h.
 * 
 ******************************************************************************/

#include "loader.h"
#include "western2208.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ensure_valid_binary(FILE* binary, char* executable)
{
	int magic_number_len = strlen(MAGIC_NUMBER);
	char buffer[magic_number_len];
	int bytes_read;
	
	bytes_read = fread(buffer, sizeof(char), magic_number_len, binary);
	
	if ((bytes_read != magic_number_len) |
	    (strncmp(buffer, MAGIC_NUMBER, magic_number_len)))
	{
		fprintf(stderr, "The file '%s' is not a valid Western 2208 executable.\n", executable);
		fclose(binary);
		exit(ERROR_INVALID_EXECUTABLE);
	}
}

FILE* open_binary(char* executable)
{
	FILE* binary = fopen(executable, "rb");  
	
	if (! binary)
	{
		fprintf(stderr, "The file '%s' does not exist.\n", executable);
		exit(ERROR_FILE_NOT_FOUND);
	}
	
	return binary;	
}

binary_header_t read_header(FILE* binary)
{
	binary_header_t header;
	int ds_size = 0,
	    ts_size = 0,
    	    ints_read;
	
	ints_read = fread(&ds_size, sizeof(int), 1, binary);

	if ((ints_read != 1) || (ds_size < 0) || (ds_size > RAM_SIZE))
	{
		fprintf(stderr, "Invalid data segment size in executable header.\n");
		fclose(binary);
		exit(ERROR_INVALID_HEADER);
	}
	
	ints_read = fread(&ts_size, sizeof(int), 1, binary);
	
	if ((ints_read != 1) || (ts_size < 0) || (ts_size > RAM_SIZE))
	{
		fprintf(stderr, "Invalid text segment size in executable header.\n");
		fclose(binary);
		exit(ERROR_INVALID_HEADER);
	}
	
	if (ds_size + ts_size == 0) 
	{
		fprintf(stderr, "No instructions found in executable.\n");
		fclose(binary);
		exit(ERROR_EMPTY_EXECUTABLE);
	}
	
	if (ds_size + ts_size > RAM_SIZE)
	{
		fprintf(stderr, "The specified executable cannot fit into RAM.\n");
		fclose(binary);
		exit(ERROR_OUT_OF_MEMORY);
	}
	
	if (ds_size % 4 != 0)
	{
		fprintf(stderr, "Alignment error: the text segment does not begin at an address divisible by 4.\n");
		fclose(binary);
		exit(ERROR_TS_ALIGNMENT);
	}
	
	header.size = ds_size + ts_size;
	header.entry_point = ds_size;
	
	return header;
}

process_t* load(char* executable)
{
	FILE* binary;
	process_t* process;
	binary_header_t header;
	int bytes_read;
	
	binary = open_binary(executable);
	ensure_valid_binary(binary, executable);
	header = read_header(binary);
	
	process = (process_t*)calloc(1, sizeof(process_t));
	bytes_read = fread(process->ram.bytes, sizeof(char), header.size, binary);
	
	if (bytes_read != header.size)
	{
		fprintf(stderr, "Executable header reports an invalid size.\n");
		free(process);
		exit(ERROR_INVALID_HEADER);
	}
	
	process->entry_point = header.entry_point;
	process->size = header.size;
	
	fclose(binary);
	return process;
}
