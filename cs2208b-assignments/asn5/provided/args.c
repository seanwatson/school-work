/*******************************************************************************
 *
 * File         : args.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * See function descriptions in args.h.
 * 
 ******************************************************************************/

#include "args.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <string.h>

bool_t has_extension(char* filename, char* ext)
{
	char* file_ext;
	int len = strlen(ext);
	
	// Get the file extension
	file_ext = strrchr(filename, '.');
	
	if ((!file_ext) || (!*(file_ext + 1)))
		return FALSE;

	return ((strlen(file_ext + 1) == len) && (!strncmp(file_ext + 1, ext, len)));
}

void show_usage(char* program_name)
{
	fprintf(stderr, "Usage: %s [OPTION]... [FILE]\n", program_name);
	fprintf(stderr, "Assembler for the CS 2208b Simple CPU\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h, --help         display this help and exit\n");
	fprintf(stderr, "  -d, --debug        print all machine code to standard output\n");
	
	exit(EXIT_FAILURE);
}

assembler_args_t parse_args(int argc, char* argv[])
{
	char c;
	int opt_index = 0;
	char *program_name = basename(argv[0]);
	assembler_args_t args;
		
	struct option long_options[] = {
	    { "debug",	no_argument,	&args.debug_flag,  TRUE },	// --debug or -d
	    { "help", 	no_argument,	NULL, 		   0  	},	// --help or -h
	    { 0,    	0,    		0,    		   0   	}	// terminating 0 item
	};

	opterr = FALSE;
	args.debug_flag = FALSE;

	while ((c = getopt_long(argc, argv, "dh", long_options, &opt_index)) != -1)
	{
		switch (c)
		{
			case 'd':
				args.debug_flag = TRUE;
				break;
				
			case 'h':
				show_usage(program_name);
				break;
				
			case '\0':
				if (strcmp("help", long_options[opt_index].name) == 0)
					show_usage(program_name);
					
				break;
				
			case '?':

				if (optopt == '\0')
					fprintf(stderr, "Invalid option '%s'\n\n", argv[optind - 1]);
				else
               				fprintf(stderr, "Invalid option '-%c'\n\n", optopt);
				
				show_usage(program_name);
				break;
		}
	}
	
	if (argc - optind != 1)
	{
		fprintf(stderr, "No input file specified\n\n");
		show_usage(program_name);
	}
	else
		args.input_file = argv[optind];
	
	if (!has_extension(args.input_file, "asm"))
	{
		fprintf(stderr, "Invalid file extension -- '.asm' expected\n");
		exit(EXIT_FAILURE);
	}
	
	return args;	
}
