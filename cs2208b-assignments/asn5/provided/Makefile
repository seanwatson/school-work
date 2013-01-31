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
LEX=flex
YACC=bison
M4=/usr/local/bin/m4
YACCOPTS=-d --defines=../include/parser.tab.h
COPTS=-I ../include -I /usr/local/include/
OBJ_FILES=parser.tab.o scanner.o args.o hash_table.o plugins.o loader.o western2208.o

# If you're working on a Mac, uncomment the line below
# (Note that you'll need to install m4 version 1.4.16 or higher using MacPorts)	
#M4=/opt/local/bin/gm4

all:	libwestern2208.a assembler_provided.o cpu.o 

libwestern2208.a: $(OBJ_FILES)
	ar rcs libwestern2208.a $^

%.tab.c: %.y
	M4=$(M4) $(YACC) $(YACCOPTS) $<

%.o:	%.c
	$(CC) $(COPTS) -g -c $<

%.c:	%.l
	M4=$(M4) $(LEX) -o $@ $<

clean:
	rm -f $(OBJ_FILES) libwestern2208.a parser.tab.h cpu.o assembler_provided.o
