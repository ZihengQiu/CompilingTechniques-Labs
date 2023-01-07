#include <stdio.h>
// #include "calc.tab.h"
#include "ast.h"

extern int yyparse();
extern past AstRoot;
extern FILE *yyin;

#define USE_FILE 1

// int main(int argc, char **argv)
#if USE_FILE
int main(int argc, char **argv)
{
	if (argc == 2)
	{
		yyin = fopen(argv[1], "r");
	}
	//yyin = fopen("input.c", "r");
	printf("Before yyparse\n");

	yyparse();

	printf("After yyparse\n");

	showNode(AstRoot, 0);

	return 0;
}
#else

int main(void)
{
	
	int a = 666;
	printf("Before yyparse\n");

	yyparse();

	printf("After yyparse\n");

	showNode(AstRoot, 0);

	return 0;
}
#endif

/*
int main(int argc, char **argv)
{
	if (argc > 2)
	{
		printf("argcs too many!.\n");
		return 0;
	}
	if (argc == 2)
	{
		yyin = fopen(argv[1], "r");
	}
	else
	{
		printf("no file input!\n");
		return 1;
	}

	int a = 666;
	printf("Before yyparse\n");

	yyparse();

	printf("After yyparse\n");

	showNode(AstRoot, 0);

	return 0;
}
https://pandolia.net/tinyc/ch8_flex.html
*/
