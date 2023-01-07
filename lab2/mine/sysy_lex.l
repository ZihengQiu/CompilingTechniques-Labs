%{

#include "token.h"

#define YYSTYPE _YYLVAL

int num_lines = 0;

%}
DIGIT		[0-9]
INT		    [0-9][0-9]*
FLOAT		{DIGIT}*(\.{DIGIT}+)?(e|E[+|-]?{DIGIT}+)?

WORD        [a-zA-Z]
ID          [a-zA-Z_][a-zA-Z0-9_]*

%% 

\n              {++num_lines;}

{INT}           {return num_INT;}
{FLOAT}         {return num_FLOAT;} 

"int"           {return Y_INT;}
"void"          {return Y_VOID;}
"const"         {return Y_CONST;}
"if"            {return Y_IF;}
"else"          {return Y_ELSE;}
"while"         {return Y_WHILE;}
"break"         {return Y_BREAK;}
"continue"      {return Y_CONTINUE;}
"return"        {return Y_RETURN;}

[+]  	          {return Y_ADD;}
[-]  	          {return Y_SUB;}
[*] 	          {return Y_MUL;}
[/]	            {return Y_DIV;}
[%]	            {return Y_MODULO;}
[<]		          {return Y_LESS;}
[<][=]	        {return Y_LESSEQ;}
[>]     	      {return Y_GREAT;}
[>][=]          {return Y_GREATEQ;}
[!][=]          {return Y_NOTEQ;}
[=][=]          {return Y_EQ;}
[!]             {return Y_NOT;}
[&][&]          {return Y_AND;}
[|][|]          {return Y_OR;}
[=]             {return Y_ASSIGN;}

\(              {return Y_LPAR;}
\)              {return Y_RPAR;}
\[              {return Y_LBRACKET;}
\]              {return Y_RBRACKET;}
\{              {return Y_LSQUARE;}
\}              {return Y_RSQUARE;}
[,]             {return Y_COMMA;}
[;]             {return Y_SEMICOLON;}

"float"         {return Y_FLOAT;}

{ID}            {return Y_ID;}

"//".*"\n"  ;
[ \t] ; 	/* skip whitespace */
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]       { /* DO NOTHING */ }

[[:space:]]		{return -1;}
<<EOF>>             { return 0; }

%%
// int main(int argc, char **argv)
// {
//     int token;
//     while(token = yylex())
//     {
//     	if(token != -1)
// 		printf("< %d , %s >\n",token, yytext);
//     }
//     printf("lines:%d", num_lines);
// }

