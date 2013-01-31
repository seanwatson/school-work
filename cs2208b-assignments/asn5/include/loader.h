/*******************************************************************************
 *
 * File         : loader.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Helper functions and definitions for the loader.  You may find the 
 * MAGIC_NUMBER macro handy when writing your executable header.
 *
 ******************************************************************************/

#include "western2208.h"

#ifndef LOADER_H
#define LOADER_H

// Load errors 
#define ERROR_FILE_NOT_FOUND -1
#define ERROR_INVALID_EXECUTABLE -2
#define ERROR_INVALID_HEADER -3
#define ERROR_EMPTY_EXECUTABLE -4
#define ERROR_OUT_OF_MEMORY -5
#define ERROR_TS_ALIGNMENT -6

// Magic number to be found at the start of a valid 2208EF (CS 2208 Executable
// Format) executable file
#define MAGIC_NUMBER "2208EF"

typedef struct
{
	int size;
	int entry_point;
} binary_header_t;

process_t* load(char* executable);

#endif
