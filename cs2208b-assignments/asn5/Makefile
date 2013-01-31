################################################################################
#                                                                              #
# Makefile                                                                     #
#                                                                              #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last Updated: March 29, 2012                                                 #
#                                                                              #
# Builds the provided code for the 2208 Western CPU and the Western 2208       #
# CPU Assembler.                                                               #
#                                                                              #
# Use the Makefile in the parent directory to build this code                  #
#                                                                              #
# Note that if you're working on a Mac, you'll need to see the uncomment the   #
# Mac settings where directed below.                                           #
#                                                                              #
################################################################################

CC=gcc
COPTS=-I include 
LOPTS=-Bstatic -lwestern2208 -L provided
MAKE=gmake

all:	.provided asm .plugins cpu 

asm:	assembler.o provided/assembler_provided.o
	$(CC) -g -o $@ $^ $(LOPTS) 

cpu:	provided/cpu.o provided/loader.o
	$(CC) -o $@ -Bdynamic -lreadline -lncurses $^ $(LOPTS)

%.o:	%.c
	$(CC) $(COPTS) -c $<

.provided:
	@$(MAKE) -C provided

.plugins:	
	@$(MAKE) -C plugins

clean:	
	rm -f *.o
	rm -f parser.tab.h parser.tab.c scanner.c parser.output
	@$(MAKE) -C plugins clean
	@$(MAKE) -C provided clean

realclean: clean
	rm -f asm cpu
