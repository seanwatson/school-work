/*******************************************************************************
 *
 * File         : scanner.l
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * Lexer for the assembler.
 * 
 ******************************************************************************/

/* Don't use yywrap (thus not requiring libfl) */
%option noyywrap

/* Don't add a default rule */
%option nodefault  

  
%{
    char string_buf[65536];
    char *string_buf_ptr;
    int last_state;
    int char_count;
%}

%x DATASEGMENT
%x STRLIT
%x CHRLIT

delimiter               [ \t]+
whitespace              {delimiter}+
digit                   [0-9]
number                  [-]?{digit}*[.]?{digit}+
integer                 [-]?{digit}+
hex                     "0x"[0-9a-fA-F]+
general_register        %r([0-9]|1[0-5])
comment                 #.*

%{
#include "assembler.h"
#include "parser.tab.h"
%}

%%

<INITIAL,DATASEGMENT>".data"                 {
                            BEGIN DATASEGMENT;
                            yylval.directive_type = DIR_DATA;
                            return SEGMENT;
                        }
                        
<INITIAL,DATASEGMENT>".text"                 {

                            BEGIN INITIAL;
                            yylval.directive_type = DIR_TEXT;
                            return SEGMENT;
                        }
                        
<INITIAL,DATASEGMENT>{integer}               { 
                            sscanf(yytext, "%d", &yylval.num); 
                            return IMMEDIATE; 
                        }
                        
<INITIAL,DATASEGMENT>{hex}                   { 
                            sscanf(yytext+2, "%x", &yylval.num);
                            return IMMEDIATE;
                        }
                                               
<INITIAL,DATASEGMENT>{general_register}      {
                            int reg_num;   
                            sscanf(yytext+2, "%d", &reg_num);
                            yylval.num = reg_num;
                            
                            return REGISTER;
                        }

<INITIAL,DATASEGMENT>"\n"                    { yylineno++; return NEWLINE; }
<INITIAL,DATASEGMENT>","                     { return COMMA; }
<INITIAL,DATASEGMENT>":"                     { return COLON; }
<INITIAL,DATASEGMENT>"+"                     { return PLUS;  }
<INITIAL,DATASEGMENT>"["                     { return INDIR_OPEN; }
<INITIAL,DATASEGMENT>"]"                     { return INDIR_CLOSE; }

<INITIAL,DATASEGMENT>".word"    { 
                            yylval.directive_type = DIR_WORD;
                            return STATIC_NUM;
                        }
                        
<INITIAL,DATASEGMENT>".half"    {
                            yylval.directive_type = DIR_HALF;
                            return STATIC_NUM;
                        }
                        
<INITIAL,DATASEGMENT>".byte"    {
                            yylval.directive_type = DIR_BYTE;
                            return STATIC_NUM;
                        }

<INITIAL,DATASEGMENT>".str"                  {
                            yylval.directive_type = DIR_STR;
                            return STATIC_STR;
                        }
                                       
<INITIAL,DATASEGMENT>".align"                {
                            yylval.directive_type = DIR_ALIGN;
                            return ALIGN;
                        }                     
             
<INITIAL,DATASEGMENT>_[A-Za-z0-9_]+          {
                            yylval.str = strdup(yytext);
                            return LABEL;
                        }

<INITIAL>(ldu?b|ldu?h|ldw) {
                            yylval.str = strdup(yytext);
                            return LOAD_OPCODE;
                        }

<INITIAL>(stb|sth|stw) {
                            yylval.str = strdup(yytext);
                            return STORE_OPCODE;
                        }

<INITIAL>[a-z]{2,4}     { 
                            yylval.str = strdup(yytext); 
                            return OPCODE;
                        }             
              
<INITIAL,DATASEGMENT>['] {
                            last_state = YY_START;
                            BEGIN CHRLIT;
                            string_buf_ptr = string_buf;
                        }
                        
<CHRLIT>[']             { 
                            if (string_buf_ptr == string_buf)
                            {
                                yyerror(1, "Empty character constant");
                                exit(EXIT_FAILURE);
                            }
                            else if (string_buf_ptr > string_buf + 1)
                            {
                                yyerror(1, "Multi-character character constant\n");
                                exit(EXIT_FAILURE);
                            }
                            else {
                                yylval.num = string_buf[0];
                                BEGIN last_state;
                                return IMMEDIATE;                                 
                            }

                        }
          
<DATASEGMENT>\"         {
                            string_buf_ptr = string_buf;
                            last_state = YY_START; 
                            BEGIN STRLIT;
                        }
                                                
<STRLIT>\"              { 
                            BEGIN last_state;
                            *string_buf_ptr = '\0';
                            yylval.str = strdup(string_buf);
                            return STRING;
                        }

<CHRLIT>\n              {
                            yyerror(1, "Unterminated character constant\n");
                            exit(EXIT_FAILURE);
                        }

<STRLIT>\n              {
                            yyerror(1, "Unterminated string constant\n");
                            exit(EXIT_FAILURE);                            
                        }
     
<STRLIT,CHRLIT>\\[0-7]{1,3} {     
                            int result;

                            sscanf(yytext + 1, "%o", &result);

                            if (result > 0xff)
                            {
                                yyerror(1, "Constant out of bounds");
                                exit(EXIT_FAILURE);
                            }

                            *string_buf_ptr++ = result;
                        }
     
<STRLIT,CHRLIT>\\[0-9]+ {
                            yyerror(1, "Bad escape sequence\n");
                            exit(EXIT_FAILURE);                            
                        }
     
<STRLIT,CHRLIT>\\n        { *string_buf_ptr++ = '\n'; }
<STRLIT,CHRLIT>\\t        { *string_buf_ptr++ = '\t'; }
<STRLIT,CHRLIT>\\r        { *string_buf_ptr++ = '\r'; }
<STRLIT,CHRLIT>\\b        { *string_buf_ptr++ = '\b'; }
<STRLIT,CHRLIT>\\f        { *string_buf_ptr++ = '\f'; }
<STRLIT,CHRLIT>\\(.|\n)   { *string_buf_ptr++ = yytext[1]; }
<STRLIT>[^\\\n\"]+ {
                            char *yptr = yytext;
     
                             while (*yptr)
                                     *string_buf_ptr++ = *yptr++;
                            
                          }  
                          
<CHRLIT>[^\\\n\']+ {
                            char *yptr = yytext;
     
                             while (*yptr)
                                     *string_buf_ptr++ = *yptr++;
                            
                          }                      
                                              

                        
<INITIAL,DATASEGMENT>{whitespace}            { /* No action and no return */ }
<INITIAL,DATASEGMENT>{comment}               { /* No action and no return */ }
<INITIAL,DATASEGMENT>.                       { return yytext[0]; }

%%