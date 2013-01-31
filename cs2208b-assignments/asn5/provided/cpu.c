/*******************************************************************************
 *
 * File         : cpu.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * See function descriptions in cpu.h.
 *
 ******************************************************************************/

#include "cpu.h"
#include "western2208.h"
#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <ctype.h>
#include "boolean.h"

cpu_t* cpu;
char* last_command = NULL;

/* The names of functions that actually do the manipulation. */
bool_t com_next(char*);
bool_t com_print_reg(char*);
bool_t com_print_mem(char*);
bool_t com_display_reg(char*);
bool_t com_registers(char*);
bool_t com_help(char*);
bool_t com_quit(char*);

/* A structure which contains information on the commands this program
   can understand. */

typedef struct {
  char *name;			/* User printable name of the function. */
  bool_t (*func)(char*);	/* Function to call to do the job. */
  char *doc;			/* Documentation for this function.  */
} COMMAND;

COMMAND commands[] = {
	{ "next", com_next, "Execute the next instruction" },
	{ "ni", com_next, ""},
	{ "n", com_next, ""},	
	{ "print", com_print_reg, "Print the value stored in a register" },
	{ "p", com_print_reg, "" },
	{ "examine", com_print_mem, "Print a value stored in RAM" },
	{ "x", com_print_mem, ""},
	{ "display", com_display_reg, "Display the value of a register on each break" },
	{ "d", com_display_reg, "" },
	{ "registers", com_registers, "Display the contents of all registers"},
	{ "r", com_registers, ""},
	{ "help", com_help, "Display this text" },
	{ "?", com_help, "" },
	{ "quit", com_quit, "Quit the debugger" },
	{ "exit", com_quit, "" },	
	{ (char *)NULL, (bool_t (*)(char*))NULL, (char *)NULL }
};

typedef struct
{
	char* name;
	uint8_t value;
} reg_map_entry_t;

reg_map_entry_t register_map[] = {

	{ "r0", REG_R0 },
	{ "r1", REG_R1 },
	{ "r2", REG_R2 },
	{ "r3", REG_R3 },
	{ "r4", REG_R4 },
	{ "r5", REG_R5 },
	{ "r6", REG_R6 },
	{ "r7", REG_R7 },
    	{ "t0", REG_TMP0 },
	{ "t1", REG_TMP1 },
	{ "t2", REG_TMP2 },
	{ "t3", REG_TMP3 },
	{ "t4", REG_TMP4 },
	{ "t5", REG_TMP5 },
	{ "t6", REG_TMP6 },
	{ "t7", REG_TMP7 },
	{ "pc", REG_PC },
	{ "ir", REG_IR },
	{ "fz", REG_FZ },
	{ "fn", REG_FN },
	{ "fv", REG_FV },
	{ "fc", REG_FC },
	{ "io", REG_IO }
};

int display_registers[REG_COUNT];
int next_display_reg = 0;

// Forward declarations
char* stripwhite();
COMMAND* find_command();
char* command_generator(const char *, int);
char** fileman_completion(const char *, int, int);

bool_t exit_requested = FALSE;

/* Execute a command line */
int execute_line(char* line)
{
  register int i;
  COMMAND* command;
  char *word;

  /* Isolate the command word */
  i = 0;
  while (line[i] && isspace(line[i]))
    i++;
  word = line + i;

  while (line[i] && !isspace(line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  command = find_command (word);

  if (!command)
    {
      fprintf (stderr, "%s: Invalid command\n", word);
      return FALSE;
    }

  /* Get argument to command, if any */
  while (isspace(line[i]))
    i++;

  word = line + i;

  /* Call the function */
  return ((*(command->func)) (word));
}

/* Look up NAME as the name of a command, and return a pointer to that
   command.  Return a NULL pointer if NAME isn't a command name */
COMMAND* find_command(char *name)
{
  register int i;

  for (i = 0; commands[i].name; i++)
    if (strcmp (name, commands[i].name) == 0)
      return (&commands[i]);

  return ((COMMAND *)NULL);
}

/* Strip whitespace from the start and end of STRING.  Return a pointer
   into STRING */
char* stripwhite(char *string)
{
  register char *s, *t;

  for (s = string; isspace(*s); s++)
    ;
    
  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && isspace(*t))
    t--;
  *++t = '\0';

  return s;
}

/* Attempt to complete on the contents of TEXT.  START and END bound the
   region of rl_line_buffer that contains the word to complete.  TEXT is
   the word to complete.  We can use the entire contents of rl_line_buffer
   in case we want to do some simple parsing.  Return the array of matches,
   or NULL if there aren't any */
char** fileman_completion(const char *text, int start, int end)
{
  char **matches;

  matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory */
  if (start == 0)
    matches = rl_completion_matches(text, command_generator);

  return (matches);
}

char* command_generator(const char *text, int state)
{
  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0 */
  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  /* Return the next name which partially matches from the command list. */
  while (name = commands[list_index].name)
    {
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (strdup(name));
    }

  /* If no names matched, then return NULL */
  return ((char *)NULL);
}

bool_t com_next(char* arg)
{
	return TRUE;
}

bool_t com_print_reg(char* arg)
{
	int i;
	
	if ((arg == NULL) || (arg[0] != '%'))
		printf("Invalid register name\n");
		
	for (i = 0; i < REG_COUNT; ++i)
	{
		if (!strncmp(register_map[i].name, arg+1, 2))
		{
			printf("%%%-10s 0x%08x (%d)\n", register_map[i].name, REG(i), REG(i));	
			return FALSE;
		}
	}
	
	printf("Invalid register name\n");	
	return FALSE;
}

bool_t com_display_reg(char* arg)
{
	int i;
	
	if ((arg == NULL) || (arg[0] != '%'))
		printf("Invalid register name\n");
	
	if (next_display_reg == REG_COUNT)
		return;
		
	for (i = 0; i < REG_COUNT; ++i)
	{
		if (!strncmp(register_map[i].name, arg+1, 2))
		{
			display_registers[next_display_reg++] = register_map[i].value;
			return FALSE;
		}
	}
	
	printf("Invalid register name\n");
	
	return FALSE;
}

void print_com_mem_args()
{
	printf("Invalid arguments.  Usage: x address/nw");
	printf("\tn Number of elements to display\n");
	printf("\tw Width of elements to display (w, h, b)\n\n");
	printf("Examples:\n");
	printf("\tx 0x20/4w - Display 4 words starting at address 0x20\n");
	printf("\tx 6/2b    - Display 2 bytes starting at address 6\n");
}
bool_t com_print_mem(char* arg)
{
	int vars_filled = 0;
	uint16_t addr;
	uint16_t num;
	char width;
	uint16_t end_addr;
	uint16_t increment;
	
	if ((arg == NULL) || (strlen(arg) < 4))
	{
		print_com_mem_args();
		return FALSE;
	}
	
	if (strncmp("0x", arg, 2))
		vars_filled = sscanf(arg, "%hu/%hu%c", &addr, &num, &width);
	else
		vars_filled = sscanf(arg, "%hx/%hd%c", &addr, &num, &width);
	
	if ((vars_filled != 3) || (addr >= RAM_SIZE))
	{
		print_com_mem_args();
		return FALSE;
	}
	
	switch(width)
	{
		case 'b':
		case 'B':
			end_addr = addr + num;
			increment = 1;
		break;
		
		case 'h':
		case 'H':
			end_addr = addr + (2 * num);
			increment = 2;
		break;
		
		case 'w':
		case 'W':
			end_addr = addr + (4 * num);	
			increment = 4;	
		break;
	}
	
	if (addr % increment != 0)
	{
		printf("Starting address must be divisible by %d when using '%c'\n", increment, width);
		return FALSE;
	}
	
	if (end_addr >= RAM_SIZE)
		end_addr = RAM_SIZE;
	
	while (addr < end_addr)
	{
		switch(width)
		{
			case 'b':
			case 'B':
				printf("[%04X]   %02X (%d)\n", addr, cpu->process->ram.bytes[addr], cpu->process->ram.bytes[addr]);
				break;
		
			case 'h':
			case 'H':
				printf("[%04X]   %04X (%d)\n", addr, cpu->process->ram.halfwords[addr / 2], cpu->process->ram.halfwords[addr / 2]);
				break;
		
			case 'w':
			case 'W':
				printf("[%04X]   %08X (%d)\n", addr, cpu->process->ram.words[addr / 4], cpu->process->ram.words[addr / 4]);
				break;
		}
		
		addr += increment;
	}
	
	return FALSE;
}

bool_t com_registers(char* arg)
{
	int i;
	
	printf("\n%-10s %s\n", "Register", "Value");
	printf("===================================\n");
	for (i = 0; i < REG_COUNT; ++i)
	{
		printf("%%%-10s 0x%08x (%d)\n", register_map[i].name, REG(i), REG(i));
	}
	
	return FALSE;
}

/* Print out help for ARG, or for all of the commands if ARG is
   not present. */
bool_t com_help(char* arg)
{
  register int i;
  int printed = 0;

  for (i = 0; commands[i].name; i++)
    {
      if (!*arg || (strcmp (arg, commands[i].name) == 0))
        {
          printf ("%-15s%s\n", commands[i].name, commands[i].doc);
          printed++;
        }
    }

  if (!printed)
    {
      printf("No commands match `%s'.  Possibilties are:\n", arg);

      for (i = 0; commands[i].name; i++)
        {
          /* Print in six columns. */
          if (printed == 6)
            {
              printed = 0;
              printf ("\n");
            }

          printf("%s\t", commands[i].name);
          printed++;
        }

      if (printed)
        printf("\n");
    }

	return FALSE;
}

bool_t com_quit (char *arg)
{
	exit_requested = TRUE;
	return FALSE;
}

void setup_debugger()
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "cpu";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = fileman_completion;

}

void show_usage(char* program_name)
{
	fprintf(stderr, "Usage: %s [PROGRAM]\n", program_name);
	fprintf(stderr, "Western 2208 microcontroller emulator\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -d, --debug        debug the specified program\n");
	fprintf(stderr, "      --disasm       disassemble the specified program\n");
	fprintf(stderr, "  -h, --help         display this help and exit\n");
	
	exit(EXIT_FAILURE);
}

cpu_args_t parse_args(int argc, char* argv[])
{
	char c;
	int opt_index = 0;
	char *program_name = basename(argv[0]);
	cpu_args_t args;
	args.debug_flag = FALSE;
	args.disasm_flag = FALSE;
		
	struct option long_options[] = {
	    { "debug",	no_argument,	&args.debug_flag,  TRUE },	// --debug or -d		
	    { "disasm",	no_argument,	&args.disasm_flag, TRUE },	// --disasm			
	    { "help", 	no_argument,	NULL, 		   0  	},	// --help or -h
	    { 0,    	0,    		0,    		   0   	}	// terminating 0 item
	};

	opterr = FALSE;

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
		fprintf(stderr, "No program specified\n\n");
		show_usage(program_name);
	}
	else
		args.filename = argv[optind];
	
	return args;
}

void print_instruction(uint16_t pc, uint32_t i)
{
	char* mnemonics[] = {
		
		"nop",  "nor", "add",  "tst", "srl", "sra", "sll", "ldb", 
		"ldub", "ldh", "lduh", "ldw", "stb", "sth", "stw"
	};

	printf("0x%04x    %-7s", pc, mnemonics[OP(i)]);

	switch(OP(i))
	{
		case INS_NOP:
			printf("\n");
			break;
		
		case INS_TST:
			printf("%%%s\n", register_map[RS1(i)].name);
			break;
			
		case INS_NOR:
		case INS_ADD:
		case INS_SRL:
		case INS_SRA:
		case INS_SLL:
			printf("%%%s, ", register_map[RS1(i)].name);
			
			if (IMM(i))
				printf("0x%x, ", RS2_OR_IMM(i));
			else
				printf("%%%s, ", register_map[RS2_OR_IMM(i)].name);
			
			printf("%%%s\n", register_map[RD(i)].name);
			break;
			
		case INS_LDB:
		case INS_LDUB:
		case INS_LDH:
		case INS_LDUH:
		case INS_LDW:
		
			printf("[%%%s", register_map[RS1(i)].name);
			
			if (RS2_OR_IMM(i) != 0)
			{			
				if (IMM(i))
					printf(" + 0x%x", RS2_OR_IMM(i));
				else
					printf(" + %%%s", register_map[RS2_OR_IMM(i)].name);	
			}
			
			printf("], %%%s\n", register_map[RD(i)].name);
			break;
			
		case INS_STB:
		case INS_STH:
		case INS_STW:
		
		printf("%%%s, [%%%s", register_map[RS1(i)].name, 
				      register_map[RD(i)].name);
			
			if (RS2_OR_IMM(i) != 0)
			{			
				if (IMM(i))
					printf(" + 0x%x", RS2_OR_IMM(i));
				else
					printf(" + %%%s", register_map[RS2_OR_IMM(i)].name);
			}
			
			printf("]\n");
			break;
		break;
		default:
			printf("Unknown opcode\n");
	}
}

void pause_execution(int sig)
{
	char *line = NULL;
	char *str;
	bool_t resume = FALSE;
	int i;
	
	signal(SIGUSR1, pause_execution);
	print_instruction(REG(REG_PC), REG(REG_IR));
	
	for (i = 0; i < next_display_reg; ++i)
	{
		printf("%%%-10s 0x%08x (%d)\n", register_map[display_registers[i]].name, REG(display_registers[i]), REG(display_registers[i]));	
	}
	
	while (!resume)
	{
		line = readline("> ");

		if (!line)
			continue;

		str = stripwhite(line);

		if (*str)
		{
			add_history(str);
			resume = execute_line(str);
			
			if (last_command)
				free(last_command);
			
			last_command = str;
		}
		else if (last_command)
		{
			resume = execute_line(last_command);
		}
	
		if (exit_requested)
		{
			if (last_command)
				free(last_command);
				
			free(cpu->process);
			free(cpu);
			exit(EXIT_SUCCESS);
		}
	}
}


void interrupt_handler(int sig)
{
	char buffer[80];
	char* error;
	
	switch(sig)
	{
		case SIGBUS:
			error = "Bus error";
			break;
			
		case SIGSEGV:
			error = "Segmentation fault";
			break;
		
		case SIGINT:
			error = "Program terminated by user";
			break;
			
		case SIGILL:
			error = "Illegal instruction";
			break;
			
		case SIGQUIT:
			error = NULL;
			break;
		
		default:
			sprintf(buffer, "Unknown error (%d)", sig);
			error = buffer;
			break;
	}
	
	if (error)
		fprintf(stderr, "%s.\n", error);

	if (cpu)
	{
		if (cpu->process)
			free(cpu->process);
			
		free(cpu);
	}
	
	exit(sig == SIGQUIT ? EXIT_SUCCESS : EXIT_FAILURE);
}

void set_interrupt_handlers(cpu_args_t args)
{
	int signals[28] = {
		SIGHUP,    SIGINT,    SIGQUIT,   SIGILL,    SIGABRT,  SIGIOT,
		SIGEMT,    SIGFPE,    SIGKILL,   SIGBUS,    SIGSEGV,  SIGSYS,
		SIGPIPE,   SIGALRM,   SIGTERM,   SIGURG,    SIGSTOP,  SIGTSTP,
		SIGCONT,   SIGCHLD,   SIGTTIN,   SIGTTOU,   SIGIO,    SIGXCPU, 
		SIGXFSZ,   SIGVTALRM, SIGPROF,   SIGUSR2
        };

        int i;

        if (args.debug_flag)
		signal(SIGUSR1, pause_execution);
	else
		signal(SIGUSR1, SIG_IGN);
	
        for (i = 0; i < 28; ++i)
                signal(signals[i], interrupt_handler);
}

void disassemble(process_t* process)
{
	uint16_t loc;
	
	for (loc = process->entry_point; loc < process->size; loc += 4)
		print_instruction(loc, process->ram.words[loc / 4]);
	
	free(process);
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	cpu_args_t args = parse_args(argc, argv);
	process_t* process = load(args.filename);
	
	if (args.disasm_flag)
		disassemble(process);
	
	cpu = schedule(process);
	set_interrupt_handlers(args);
	
	if (args.debug_flag)
		setup_debugger();
		
	run(cpu);
	raise(SIGQUIT);
}
