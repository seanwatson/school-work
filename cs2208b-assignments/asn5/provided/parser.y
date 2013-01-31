/*******************************************************************************
 *
 * File         : parser.y
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * Parser for the assembler.
 * 
 ******************************************************************************/

%start program

%{
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/parser.h"
#include "../include/assembler.h"
#include "../include/assembler_provided.h"
#include "../include/boolean.h"

int yylex(void);

%}

%union {
    int num;
    char* str;
    operand_t operand;
    instruction_t instruction;
    directive_t directive;
    directive_type_t directive_type;
    statement_t statement;
}

%parse-param { int pass }

/* standard tokens */
%token NEWLINE
%token COMMA 
%token COLON 
%token PLUS
%token INDIR_OPEN
%token INDIR_CLOSE
%token<num> REGISTER
%token<str> LABEL
%token<str> LOAD_OPCODE
%token<str> STORE_OPCODE
%token<str> OPCODE
%token<num> IMMEDIATE
%token<str> STRING;
%token<directive_type> SEGMENT
%token<directive_type> STATIC_NUM
%token<directive_type> STATIC_STR
%token<directive_type> ALIGN;
%token<directive_type> STR;

%type<directive> segment;
%type<directive> static_var;
%type<directive> alignment;
%type<operand> register;
%type<operand> immediate;
%type<operand> string;
%type<statement> statement;
%type<instruction> instruction;

%%

program: 
        |       program line
        ;
        
line:           NEWLINE
        |       statement NEWLINE { 
                    
                    if ($1.label != NULL)
                        handle_label(pass, $1.label);
                    
                    if ($1.is_instruction)
                        handle_instruction(pass, $1.instruction);
                    else if ($1.is_directive)
                        handle_directive(pass, $1.directive);
                    
                }
        ;
                        
alignment:              ALIGN immediate {
                            directive_t directive;
                            
                            directive.type = $1;
                            directive.has_operand = TRUE;
                            directive.operand = $2;
                            
                            $$ = directive;
                        }                        
                        
statement:      segment     { 
                                statement_t stmt; 
                                stmt.label = NULL;
                                stmt.is_instruction = FALSE;
                                stmt.is_directive = TRUE;
                                stmt.directive = $1;
                                
                                $$ = stmt;
                            }
                            
         |      alignment   {
                                statement_t stmt;
                                stmt.label = NULL;
                                stmt.is_instruction = FALSE;
                                stmt.is_directive = TRUE;
                                stmt.directive = $1;
                                
                                $$ = stmt;
                            }
                            
         |      LABEL COLON {
                                statement_t stmt;
                                stmt.label = $1;
                                stmt.is_instruction = FALSE;
                                stmt.is_directive = FALSE;
                                
                                $$ = stmt;
                            }
                            
         |      instruction {
                                statement_t stmt;
                                stmt.label = NULL;
                                stmt.is_instruction = TRUE;
                                stmt.is_directive = FALSE;
                                stmt.instruction = $1;
                                
                                $$ = stmt;
                            }
                            
         |      static_var  {
                                statement_t stmt;
                                stmt.label = NULL;
                                stmt.is_instruction = FALSE;
                                stmt.is_directive = TRUE;
                                stmt.directive = $1;
                                
                                $$ = stmt;
                            }
                            
         |      LABEL COLON instruction {
                                statement_t stmt;
                                stmt.label = $1;
                                stmt.is_instruction = TRUE;
                                stmt.is_directive = FALSE;
                                stmt.instruction = $3;
                                
                                $$ = stmt;
                            }
                            
         |      LABEL COLON static_var {
                                statement_t stmt;
                                stmt.label = $1;
                                stmt.is_instruction = FALSE;
                                stmt.is_directive = TRUE;
                                stmt.directive = $3;
        
                                $$ = stmt;
                            }
         ;
                
register:       REGISTER
                {
                    operand_t op;
                    
                    op.type = OP_REGISTER;
                    op.value.integer = $1;
                
                    $$ = op;
                }
                ;
               
immediate:      IMMEDIATE
                {
                    operand_t op;
                    
                    op.type = OP_IMMEDIATE;
                    op.value.integer = $1;

                    $$ = op;
                }
                ;
                
string:         STRING
                {
                    operand_t op;
                    
                    op.type = OP_STRING;
                    op.value.string = $1;
                    
                    $$ = op;
                }
                ;
                                
instruction:    OPCODE 
                {
                    instruction_t i;  
                    i.mnemonic = $1; 
                    i.operand_count = 0;

                    $$ = i;                       
                }
         |
                OPCODE register
                { 
                    instruction_t i;  
                    i.mnemonic = $1; 
                    i.operand_count = 1;
                    i.operands[0] = $2;

                    $$ = i;
                }
                
         |      OPCODE LABEL 
                { 
                    instruction_t i;  
                    i.mnemonic = $1; 
                    i.operand_count = 1;
                    i.operands[0].type = OP_LABEL;
                    i.operands[0].value.string = $2;
                    
                    $$ = i;
                }
                
         |      OPCODE LABEL COMMA register
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 2;
                    i.operands[0].type = OP_LABEL;
                    i.operands[0].value.string = $2;
                    i.operands[1] = $4;
                    
                    $$ = i;
                }
                
         |      OPCODE register COMMA register
                {
                    instruction_t i;  
                    i.mnemonic = $1; 
                    i.operand_count = 2;
                    
                    i.operands[0] = $2;
                    i.operands[1] = $4;                    
                    
                    $$ = i;
                }
                
         |      OPCODE immediate COMMA register
                {
                    instruction_t i;  
                    i.mnemonic = $1; 
                    i.operand_count = 2;
                    
                    i.operands[0] = $2;
                    i.operands[1] = $4;                    
                    
                    $$ = i;
                }                
                      
         |      OPCODE register COMMA register COMMA register
                {
                    instruction_t i;  
                    i.mnemonic = $1; 
                    i.operand_count = 3;
                    
                    i.operands[0] = $2;
                    i.operands[1] = $4;
                    i.operands[2] = $6;
                    
                    $$ = i;
                }
                
         |      OPCODE register COMMA immediate COMMA register
                {
                    instruction_t i;  
                    i.mnemonic = $1; 
                    i.operand_count = 3;
                    
                    i.operands[0] = $2;
                    i.operands[1] = $4;
                    i.operands[2] = $6;
                    
                    $$ = i;
                }
         
         |      LOAD_OPCODE INDIR_OPEN register INDIR_CLOSE COMMA register                
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 2;
                    
                    i.operands[0] = $3;
                    i.operands[1] = $6;
                    
                    $$ = i;
                }
                
         |      LOAD_OPCODE INDIR_OPEN register PLUS register INDIR_CLOSE COMMA register                
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 3;
                    
                    i.operands[0] = $3;
                    i.operands[1] = $5;
                    i.operands[2] = $8;
                    
                    $$ = i;
                }

         |      LOAD_OPCODE INDIR_OPEN register PLUS immediate INDIR_CLOSE COMMA register                
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 3;
                    
                    i.operands[0] = $3;
                    i.operands[1] = $5;
                    i.operands[2] = $8;
                    
                    $$ = i;
                }

         |      LOAD_OPCODE INDIR_OPEN register PLUS LABEL INDIR_CLOSE COMMA register                
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 3;
                    
                    i.operands[0] = $3;
                    i.operands[1].type = OP_LABEL;
                    i.operands[1].value.string = $5;
                    i.operands[2] = $8;
                    
                    $$ = i;
                }

         |      STORE_OPCODE register COMMA INDIR_OPEN register INDIR_CLOSE
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 2;
                    
                    i.operands[0] = $2;
                    i.operands[1] = $5;
                    
                    $$ = i;
                }
                
         |      STORE_OPCODE register COMMA INDIR_OPEN register PLUS register INDIR_CLOSE
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 3;
                    
                    i.operands[0] = $2;
                    i.operands[1] = $7;
                    i.operands[2] = $5;
                    
                    $$ = i;
                }

         |      STORE_OPCODE register COMMA INDIR_OPEN register PLUS immediate INDIR_CLOSE
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 3;
                    
                    i.operands[0] = $2;
                    i.operands[1] = $7;
                    i.operands[2] = $5;
                    
                    $$ = i;
                }   
         |      STORE_OPCODE register COMMA INDIR_OPEN register PLUS LABEL INDIR_CLOSE
                {
                    instruction_t i;
                    i.mnemonic = $1;
                    i.operand_count = 3;
                    
                    i.operands[0] = $2;
                    i.operands[1].type = OP_LABEL;
                    i.operands[1].value.string = $7;
                    i.operands[2] = $5;
                    
                    $$ = i;
                }   
         ;

segment:        SEGMENT
                {
                    directive_t directive;
                    
                    directive.type = $1;
                    directive.has_operand = FALSE;

                    $$ = directive;
                }
                
static_var:     STATIC_NUM immediate
                {
                    directive_t directive;
                    
                    directive.type = $1;
                    directive.has_operand = TRUE;
                    directive.operand = $2;
                    
                    $$ = directive;
                }
                
            |   STATIC_STR string
                {
                    directive_t directive;
                    
                    directive.type = $1;
                    directive.has_operand = TRUE;
                    directive.operand = $2;
                    
                    $$ = directive;
                }
         ;

%%

int yyerror(int pass, const char* err)
{
        fprintf(stderr, "%s on line %d: '%s'\n", err, yylineno, yytext);
}
