/*******************************************************************************
 *
 * File         : parser.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Helper functions for the parser.  You won't be working with these files.
 *
 ******************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#define FIRST_PASS 1
#define SECOND_PASS 2

extern int yylineno;
extern char* yytext;
int pass;

int yyerror(int pass, const char* err);

#endif
