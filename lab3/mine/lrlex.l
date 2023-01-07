%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "lrparser.tab.h"

void yyerror(char *);

extern int lines;

%}

DIGIT		[0-9]
INT		    [0-9][0-9]*
/* FLOAT		{DIGIT}*(\.{DIGIT}+)?(e|E[+|-]?{DIGIT}+)?*/
FLOAT		^[-]?[0-9]+(\.[0-9]+)?$

WORD        [a-zA-Z]
ID          [a-zA-Z_][a-zA-Z0-9_]*

%% 


{INT}           {yylval.iValue = atoi(yytext); return num_INT;}
{FLOAT}         {yylval.iValue = atoi(yytext); return num_FLOAT;} 

"int"           {return INT;}
"float"         {return FLOAT;}
"void"          {return VOID;}
"const"         {return CONST;}
"if"            {return IF;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"break"         {return BREAK;}
"continue"      {return CONTINUE;}
"return"        {return RETURN;}

[+]  	        {return ADD;}
[-]				{return SUB;}
[*] 	        {return MUL;}
[/]	            {return DIV;}
[%]	            {return MODULO;}
[<]		        {return LESS;}
[<][=]	        {return LESSEQ;}
[>]     	    {return GREAT;}
[>][=]          {return GREATEQ;}
[!][=]          {return NOTEQ;}
[=][=]          {return EQ;}
[!]             {return NOT;}
[&][&]          {return AND;}
[|][|]          {return OR;}
[=]             {return ASSIGN;}

\(              {return LPAR;}
\)              {return RPAR;}
\[              {return LSQUARE;}
\]              {return RSQUARE;}
\{              {return LBRACKET;}
\}              {return RBRACKET;}
[,]             {return COMMA;}
[;]             {return SEMICOLON;}

{ID}            {yylval.sValue = strdup(yytext); return ID;}

[\n] 			{lines++;}
[-+*/\n] 		return *yytext;
[ \t] ; 	/* skip whitespace */
[[:space:]]	{return -1;}
"//".*"\n"  ;
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]       { /* DO NOTHING */ }

<<EOF>>             { return 0; }

. yyerror("invalid character");

%%

int yywrap(void) 
{
	return 1;
}
