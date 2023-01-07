%{

#include <stdio.h>
int yylex(void);
void yyerror(char *);

void show(char* str)
{
	printf("str: %s \n", str);
}

%}

%token NUMBER

%%

program: program expr '\n'      { show("program"); printf("%d\n",  $2); }
		|
		;

expr: factor			  {show("expr1"); $$ = $1;}
	| expr '+' factor     {show("expr2"); $$ = $1 + $3;}
	| expr '-' factor     {show("expr3"); $$ = $1 - $3;}
	;
	
factor: term			 {show("factor1"); $$ = $1;}
	| factor '*' term    {show("factor2"); $$ = $1 * $3;} 
	| factor '/' term    {show("factor3"); $$ = $1 / $3;}
	;
	
term: NUMBER            {show("term1"); $$ = $1;}
	| '-' term          {show("term2"); $$ = -$2; }
	;

%%
void yyerror(char *s) 
{
	fprintf(stderr, "%s\n", s);
}
int main(void) 
{
    yyparse();
    return 0;
}
