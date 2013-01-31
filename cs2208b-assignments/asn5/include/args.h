/*******************************************************************************
 *
 * File         : args.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Function definitions for parsing command line arguments to the assembler.
 * You don't need to work with this file.
 *
 ******************************************************************************/

#include "assembler.h"

#ifndef ARGS_H
#define ARGS_H

assembler_args_t parse_args(int argc, char* argv[]);
void show_usage(char* program_name);

#endif
