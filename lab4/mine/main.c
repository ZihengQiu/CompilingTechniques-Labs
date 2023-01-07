#include <stdio.h>
#include "ast.h"
#include "genllvm.h"

extern int yyparse();
extern past AstRoot;
extern FILE *yyin;

extern int temVarNum;
past astRoot;

int main(int argc, char **argv)
{
	if (argc < 2)
		yyin = fopen("./expr.txt", "r");
	else
		yyin = fopen(argv[1], "r");

	yyin = fopen("./expr.c", "r");
	if (yyparse())
	{
		printf("Error ! Error ! Error !\n");
		return -1;
	}
	fclose(yyin);

	//showNode(AstRoot, 0);

	genNode(AstRoot, NULL);
	showCode();

	return 0;
}

