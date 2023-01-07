#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "node_type.h"

extern int yylex();
_YYLVAL yylval;
extern char *yytext;
extern FILE *yyin;

struct _token
{
	int tok;
	_YYLVAL my_yylval;
	char *my_yytext;
};
typedef struct _token token;

token tok1, tok2, tok3;

int path = 0;

#define __DEBUG 1

void showPath()
{
#if __DEBUG
	path++;
	printf("path: %d ", path);
	printf("token: %d\n", tok1.tok);
#else

#endif
}

void showYYTEXT()
{
	printf("oper : %s\n", yytext);
}

void advance()
{
	//free(tok1.my_yytext);
	tok1 = tok2;
	tok2 = tok3;
	tok3.tok = yylex();
	int times = 0;
	while(tok3.tok == -1 && times<100){
		tok3.tok = yylex();
		times++;
	}
	//tok3.my_yylval = yylval;
	// tok3.my_yylval.int_value = yylval.int_value;
	// tok3.my_yylval.float_value = yylval.float_value;
	tok3.my_yytext = strdup(yytext);
	// printf("tok: %s\n", yytext);
}

typedef struct _ast ast;
typedef struct _ast *past;
struct _ast
{
	int ivalue;
	float fvalue;
	char *svalue;
	char *nodeTypeStr;
	node_type nodeType;
	past left;
	past right;
	past if_cond;
	past next;
};

past astAddExp();
past astLOrExp();
past astExp();
past astLVal();
past astStmt();
past astDecl();
past astBlock();
past astBlockItem();
past astBlockItems();
past astFuncDef();
past astFuncFParam();
past astConstDecl();
past astConstDef();
past astConstInitVal();
past astConstExp();
past astVarDecl();
past astVarDef();
past astInitVal();
past astFuncFParam();

void showTok();
void myDebug(char *s)
{
	printf("Debug in %-10s\n", s);
	printf("Token in %s\n", s);
	showTok();
}

past newAstNode()
{
	past node = malloc(sizeof(ast));
	if (node == NULL)
	{
		printf("run out of memory.\n");
		exit(0);
	}
	memset(node, 0, sizeof(ast));
	return node;
}

past newNum(int value)
{
	past var = newAstNode();
	var->nodeType = Y_INT;
	var->nodeTypeStr = "Y_INT";
	var->ivalue = value;
	// var -> next = next;
	return var;
}

past newBinaryExp(int oper, past left, past right, past next)
{
	past var = newAstNode();
	var->nodeType = BINARY_OPERATOR;
	var->nodeTypeStr = "BINARY_OPERATOR";
	var->ivalue = oper;
	var->left = left;
	var->right = right;
	var->next = next;
	return var;
}

/*  1 CompUnit: Decl CompUnit
	2         | FuncDef CompUnit
	3         | Decl
	4         | FuncDef

	CompUnit → [ CompUnit ] ( Decl | FuncDef )

	Decl: ConstDecl | VarDecl (const int float)
	FuncDef : 'void' | 'int' | 'float' Ident '('
*/

past astCompUnit()
{
	past root = newAstNode();
	root->nodeType = TRANSLATION_UNIT;
	root->nodeTypeStr = "TRANSLATION_UNIT";
	past node = newAstNode();
	past node2 = newAstNode();	// node->node2...
	int IsFirst = 1;
	while (tok1.tok == Y_CONST || tok1.tok == Y_INT || tok1.tok == Y_FLOAT || tok1.tok == Y_VOID)
	{
		if ((tok1.tok == Y_INT || tok1.tok == Y_FLOAT || tok1.tok == Y_VOID) && tok2.tok == Y_ID && tok3.tok == Y_LPAR)
		{
			node2 = astFuncDef();
		}
		else
		{
			node2 = astDecl();
		}
		if(IsFirst == 1){
			IsFirst = 0;
			root->left = node2;
			node = node2;
		} else {
			node->next = node2;
			node = node2;
		}
	}
	return root;
}

/*
    5 Decl: ConstDecl
    6     | VarDecl
*/
past astDecl()
{
	past root = newAstNode();
	root->nodeType = DECL_STMT;
	root->nodeTypeStr = "DECL_STMT";
	if(tok1.tok == Y_CONST)
	{
		//ConstDecl
		advance(); // BType
		root->ivalue = Y_CONST; // int or float need to be chosen
		past ConstDecl = newAstNode();
		ConstDecl = astConstDecl();
		root->left = ConstDecl;
	}else{
		//VarDecl
		//tok = type
		//advance();
		root->ivalue = Y_INT;
		past VarDecl = newAstNode();
		VarDecl = astVarDecl();
		root->left = VarDecl;
	}
	return root; 
}
/*
    7 ConstDecl: Y_CONST Type ConstDef Y_SEMICOLON
    8          | Y_CONST Type ConstDefs Y_SEMICOLON

	ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
*/
past astConstDecl()
{
	past FirstConstDef = newAstNode();
	if(tok1.tok == Y_INT){	// BType
		FirstConstDef->svalue = "const int";
	}else{
		FirstConstDef->svalue = "const float";
	}
	advance(); // tok = constdef
	// pay attention to the sequence ! first is first
	FirstConstDef = astConstDef();
	past ConstDef = FirstConstDef;
	past NextConstDef = newAstNode();
	while(tok1.tok == Y_COMMA)
	{
		advance();
		NextConstDef = astConstDef();
		ConstDef->next = NextConstDef;
		ConstDef = NextConstDef;
	}
	if(tok1.tok != Y_SEMICOLON)
	{
		printf("Error in ConstDef expect Y_SEMICOLON\n");
		return NULL;
	}
	advance();
	return FirstConstDef;
}
/*
    9 ConstDefs: ConstDef Y_COMMA ConstDef
    10          | ConstDefs Y_COMMA ConstDef
*/

/*
    11 ConstDef: Y_ID 			Y_ASSIGN ConstInitVal
    12         | Y_ID ConstExps Y_ASSIGN ConstInitVal

	ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
*/

past astConstDef()
{
	past root = newAstNode();
	root->svalue = tok1.my_yytext; //tok = Y_ID
	root->nodeType =  VAR_DECL;
	root->nodeTypeStr = "VAR_DECL";

	advance(); // tok = Y_ASSIGN or Y_LBRACKET
	int times = 0;
	while (tok1.tok == Y_LBRACKET)
	{
		advance();
		times++;
		past ConstExp = newAstNode();
		ConstExp = astConstExp();

		if (tok1.tok != Y_RBRACKET)
		{
			printf("ERROR in VarDef, expect Y_RBRACKET\n");
			return NULL;
		}
		root->svalue = strcat(root->svalue, "[]");

		// tok = Y_RBRACKET
		advance();
	}
	if(tok1.tok != Y_ASSIGN){
		printf("ERROR in ConstDel, expect Y_ASSIGN\n");
		return NULL;
	}
	advance(); // tok = ConstInitVal
	past ConstInitVal = newAstNode();
	ConstInitVal = astConstInitVal();
	root->left = ConstInitVal;
	return root;
}

/*
    13 ConstExps: Y_LSQUARE ConstExp Y_RSQUARE
    14          | Y_LSQUARE ConstExp Y_RSQUARE ConstExps
*/
/*
    15 ConstInitVal: ConstExp
    16             | Y_LBRACKET Y_RBRACKET
    17             | Y_LBRACKET ConstInitVal Y_RBRACKET
    18             | Y_LBRACKET ConstInitVal ConstInitVals Y_RBRACKET

	ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
*/

past astConstInitVal()
{
	past root = newAstNode();
	if(tok1.tok == Y_LSQUARE)
	{
		root->nodeType = INIT_LIST_EXPR;
		root->nodeTypeStr = "INIT_LIST_EXPR";
		//'{' [ ConstInitVal  ',' ConstInitVal  ',' ConstInitVal  ] '}'
		advance();
		if(tok1.tok == Y_RSQUARE)
		{
			// tok = Y_RSQUARE
			advance();
			return root;
		}else{
			// tok = ConstInitVal
			past FirstConstInitVal = newAstNode();
			FirstConstInitVal = astConstInitVal();

			past ConstInitVal = FirstConstInitVal;
			past NextConstInitVal = newAstNode();
			while(tok1.tok == Y_COMMA)
			{
				advance();
				NextConstInitVal = astConstInitVal();
				ConstInitVal->next = NextConstInitVal;
				ConstInitVal = NextConstInitVal;
			}
			if(tok1.tok != Y_RSQUARE)
			{
				printf("Error in ConstInitVal expect Y_RSQUARE\n");
				return NULL;
			}
			advance();
			root->left = FirstConstInitVal;
			return root;
		}
	}
	else
	{
		root =  astConstExp();
		return root;
	}
}

/*
    19 ConstInitVals: Y_COMMA ConstInitVal
    20              | Y_COMMA ConstInitVal ConstInitVals
*/


/*
    21 VarDecl: Type VarDef Y_SEMICOLON
    22        | Type VarDef VarDecls Y_SEMICOLON

	VarDecl → BType VarDef { ',' VarDef } ';'
*/

past astVarDecl()
{
	past FirstVarDef = newAstNode();
	if(tok1.tok == Y_INT){	// BType
		FirstVarDef->svalue = "int";
	}else{
		FirstVarDef->svalue = "float";
	}
	advance(); // tok = VarDef
	// pay attention to the sequence ! first is first
	FirstVarDef = astVarDef();
	past VarDef = FirstVarDef;
	past NextVarDef = newAstNode();
	while(tok1.tok == Y_COMMA)
	{
		advance();
		NextVarDef = astVarDef();
		VarDef->next = NextVarDef;
		VarDef = NextVarDef;
	}
	if(tok1.tok != Y_SEMICOLON)
	{
		printf("Error in VarDef expect Y_SEMICOLON\n");
		showTok();
		return NULL;
	}
	advance();
	return FirstVarDef;
}

/*
    23 VarDecls: Y_COMMA VarDef
    24         | Y_COMMA VarDef VarDecls
*/
/*
    25 VarDef: Y_ID
    26       | Y_ID Y_ASSIGN InitVal
    27       | Y_ID ConstExps
    28       | Y_ID ConstExps Y_ASSIGN InitVal

	VarDef →  Ident { '[' ConstExp ']' }
			| Ident { '[' ConstExp ']' } '=' InitVal
*/

past astVarDef()
{
	past root = newAstNode();
	root->svalue = tok1.my_yytext; //tok = Y_ID
	root->nodeType =  VAR_DECL;
	root->nodeTypeStr = "VAR_DECL";
	
	advance(); // tok = Y_ASSIGN or Y_LBRACKET
	int times = 0;
	while(tok1.tok == Y_LBRACKET)
	{
		advance(); // tok = ConstExp
		times++;
		past ConstExp = newAstNode();
		ConstExp = astConstExp();
		
		if(tok1.tok != Y_RBRACKET)
		{
			printf("ERROR in VarDef, expect Y_RBRACKET\n");
			showTok();
			return NULL;
		}
		root->svalue = strcat(root->svalue, "[]");

		//tok = Y_RBRACKET
		advance();
	}
	if(tok1.tok != Y_ASSIGN){
		return root;
	}
	advance(); // tok = InitVal
	past InitVal = newAstNode();
	InitVal = astInitVal();
	root->left = InitVal;
	printf("InitVal:%d\n", InitVal->nodeType);
	return root;
}

/*
    29 InitVal: Exp
    30        | Y_LBRACKET Y_RBRACKET
    31        | Y_LBRACKET InitVal Y_RBRACKET
    32        | Y_LBRACKET InitVal InitVals Y_RBRACKET


	InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
*/

past astInitVal()
{
	past root = newAstNode();
	if(tok1.tok == Y_LSQUARE)
	{
		//'{' [ InitVal  ',' InitVal  ',' InitVal  ] '}'
		advance(); // tok = InitVal or Y_RSQUARE
		root->nodeType = INIT_LIST_EXPR;
		root->nodeTypeStr = "INIT_LIST_EXPR";
		if(tok1.tok == Y_RSQUARE)
		{
			// int a[1] = [];
			// tok = Y_RSQUARE
			advance();
			return root;
		}else{
			// a[1] = [1,2,...];
			// tok = InitVal
			past FirstInitVal = newAstNode();
			FirstInitVal = astInitVal();

			past InitVal = FirstInitVal;
			past NextInitVal = newAstNode();
			while(tok1.tok == Y_COMMA)
			{
				advance();// tok = InitVal
				NextInitVal = astInitVal();
				InitVal->next = NextInitVal;
				InitVal = NextInitVal;
			}
			if(tok1.tok != Y_RSQUARE)
			{
				printf("Error in InitVal expect Y_RSQUARE\n");
				return NULL;
			}
			advance();
			root->left = FirstInitVal;
			return root;
		}
	}else{
		root = astExp();
		return root;
	}
}

/*
    33 InitVals: Y_COMMA InitVal
    34         | Y_COMMA InitVal InitVals
*/

/*
	35 FuncDef: Type Y_ID Y_LPAR Y_RPAR Block
	36        | Type Y_ID Y_LPAR FuncParams Y_RPAR Block

	FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
*/
past astFuncDef()
{
	past root = newAstNode();
	if(tok1.tok == Y_INT){	// BType
		root->svalue = "int";
	}else if(tok1.tok == Y_FLOAT){
		root->svalue = "float";
	}else{
		root->svalue = "void";
	}
	advance(); // tok = Y_ID
	root->nodeType = FUNCTION_DECL;
	root->nodeTypeStr = "FUNCTION_DECL";
	root->svalue = tok1.my_yytext;
	advance(); // tok = Y_LPAR
	if(tok1.tok != Y_LPAR)
	{
		printf("ERROR in FuncDef, expect Y_LPAR\n");
		return NULL;
	}
	advance(); // tok = Y_RPAR or FuncParams
	if (tok1.tok == Y_RPAR)
	{	// no param
		advance(); // tok = Block
	}else{
		// have param
		past FirstFuncFParam = newAstNode();
		FirstFuncFParam = astFuncFParam();
		past FuncFParam = FirstFuncFParam;
		past NextFuncFParam = newAstNode();

		while(tok1.tok == Y_COMMA)
		{
			advance(); // tok = FuncFParam
			NextFuncFParam = astFuncFParam();
			FuncFParam->next = NextFuncFParam;
			FuncFParam = NextFuncFParam;
		}

		if (tok1.tok != Y_RPAR)
		{
			printf("ERROR in FuncDef, expect Y_RPAR\n");
			showTok();
			return NULL;
		}
		root->left = FirstFuncFParam;
		advance();// tok = Block
	}
		past Block = newAstNode();
		Block = astBlock();
		root->right = Block;
		return root;
}
/*
	37 FuncFParams: FuncFParam
	38           | FuncFParams Y_COMMA FuncFParam
*/

/*
	39 FuncFParam: Type Y_ID
	40          | Type Y_ID Y_LSQUARE Y_RSQUARE
	41          | Type Y_ID ArraySubscripts
	42          | Type Y_ID Y_LSQUARE Y_RSQUARE ArraySubscripts

	FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }]
*/
past astFuncFParam()
{
	past root = newAstNode();
	root->nodeType = PARM_DECL;
	root->nodeTypeStr = "PARM_DECL";
	if(tok1.tok == Y_INT){	// BType
		root->svalue = "int";
	}else {
		root->svalue = "float";
	}
	advance(); // tok = Y_ID
	root->svalue = tok1.my_yytext;
	advance(); // tok = Y_LBRACKET or Y_RBRACKET or ArraySubscripts
	if (tok1.tok == Y_LBRACKET)
	{
		// [][Exp][Exp][Exp]
		advance(); // tok = Y_RBRACKET
		if (tok1.tok != Y_RBRACKET)
		{
			printf("ERROR in FuncFParam, expect Y_RSQUARE\n");
			return NULL;
		}
		advance(); // tok = ArraySubscripts
		while (tok1.tok == Y_LBRACKET)
		{
			advance(); // tok = Exp
			past Exp = newAstNode();
			Exp = astExp();
			//root->left = ArraySubscripts;
			if (tok1.tok != Y_RBRACKET)
			{
				printf("ERROR in FuncFParam, expect Y_RSQUARE\n");
				return NULL;
			}
			advance();
		}
	}
	return root;
}
/*
    43 Block: Y_LBRACKET BlockItems Y_RBRACKET
    44      | Y_LBRACKET Y_RBRACKET

	Block → '{' { BlockItem } '}'
	Block → '{' BlockItem BlockItem BlockItem ...  '}'
*/
past astBlock()
{
	if(tok1.tok == Y_LSQUARE){
		//nodes connected with next 
		advance(); // tok = BlockItem
		past root = newAstNode();
		root->nodeType = COMPOUND_STMT;
		root->nodeTypeStr = "COMPOUND_STMT";

		past BlockItem = astBlockItem();
		root->left = BlockItem;

		past NextBlockItem = newAstNode();
		while(tok1.tok != Y_RSQUARE){
			NextBlockItem = astBlockItem();
			BlockItem->next = NextBlockItem;
			BlockItem = NextBlockItem;
			//advance();
		}
		// tok = Y_RBRACKET
		advance(); // next  token
		return root;
	}
	printf("ERROR in Block\n");
	return NULL;
}
/*
    45 BlockItems: BlockItem
    46           | BlockItem BlockItems
*/

// past astBlockItems()
// {

// }

/*
    47 BlockItem: Decl
    48          | Stmt

	BlockItem → Decl | Stmt
*/

past astBlockItem()
{
	past root = NULL;
	if(tok1.tok == Y_CONST || tok1.tok == Y_INT || tok1.tok == Y_FLOAT){
		// Decl
		root = astDecl();
	}else{
		// Stmt
		root = astStmt();
	}
	return root;
}
/*
	49 Stmt: LVal Y_ASSIGN Exp Y_SEMICOLON
	50     | Y_SEMICOLON
	51     | Exp Y_SEMICOLON
	52     | Block
	53     | Y_WHILE Y_LPAR LOrExp Y_RPAR Stmt
	54     | Y_IF Y_LPAR LOrExp Y_RPAR Stmt
	55     | Y_IF Y_LPAR LOrExp Y_RPAR Stmt Y_ELSE Stmt
	56     | Y_BREAK Y_SEMICOLON
	57     | Y_CONTINUE Y_SEMICOLON
	58     | Y_RETURN Exp Y_SEMICOLON
	59     | Y_RETURN Y_SEMICOLON

	Stmt →  LVal '=' Exp ';' | [Exp] ';' | Block
			| 'if' '( Cond ')' Stmt [ 'else' Stmt ]
			| 'while' '(' Cond ')' Stmt
			| 'break' ';'
			| 'continue' ';'
			| 'return' [Exp] ';'
*/

past astStmt()
{
	past root = newAstNode();
	switch (tok1.tok)
	{
		case Y_IF:	
		{
			// 'if' '( Cond ')' Stmt [ 'else' Stmt ]
			// if_cond + if_body (+ else_body)
			advance(); // tok = (
			if(tok1.tok != Y_LPAR){
				printf("ERROR in stmt\n");
				return NULL;
			}
			advance(); // tok = condition
			root->nodeType = IF_STMT;
			root->nodeTypeStr = "IF_STMT";
			root->ivalue = Y_IF;
			past condition = astLOrExp();
			root->if_cond = condition;

			if(tok1.tok != Y_RPAR){
				printf("ERROR in if stmt, expect Y_RPAR\n");
				showTok();
				return NULL;
			}
			advance();

			past ifbody = astStmt();
			root->left = ifbody;

			printf("%d\n", ifbody->nodeType);
			if(tok1.tok == Y_ELSE){
				advance();
				past else_body = astStmt();
				root->right = else_body;
			}
			return root;
		}
		case Y_WHILE:
		{
			//'while' '(' Cond ')' Stmt
			//if_cond + left child (without right child)
			advance(); // tok = (
			if(tok1.tok != Y_LPAR){
				printf("ERROR in while stmt, expect Y_LPAR\n");
				return NULL;
			}
			advance();//tok = Condition
			root->nodeType = WHILE_STMT;
			root->nodeTypeStr = "WHILE_STMT";
			root->ivalue = Y_WHILE;
			past condition = astLOrExp();
			root->if_cond = condition;

			//advance();
			if(tok1.tok != Y_RPAR){
				printf("ERROR in while stmt, expect Y_RPAR\n");
				return NULL;
			}
			advance();

			past while_body = astStmt();
			root->left = while_body;

			return root;
		}
		case Y_BREAK:
		{
			advance();
			root->nodeType = BREAK_STMT;
			root->nodeTypeStr = "BREAK_STMT";
			return root;
		}
		case Y_CONTINUE:
		{
			advance();
			root->nodeType = CONTINUE_STMT;
			root->nodeTypeStr = "CONTINUE_STMT";
			return root;
		}
		case Y_RETURN:
		{
			//'return' [Exp] ';'
			advance(); // tok after return
			root->nodeType = RETURN_STMT;
			root->nodeTypeStr = "RETURN_STMT";

			if(tok1.tok != Y_SEMICOLON)
			{
				past returnExp  = newAstNode();
				returnExp = astExp();
				root->left = returnExp;
			}

			if(tok1.tok != Y_SEMICOLON){
				printf("ERROR in return stmt, expect Y_SEMICOLON\n");
				return NULL;
			}
			advance();

			return root;
		}
		case Y_ID:
		{
			//LVal '=' Exp ';' 
			showTok();
			past LeftCHild = astLVal();
			past root = newAstNode();
			root->nodeType = BINARY_OPERATOR;
			root->nodeTypeStr = "BINARY_OPERATOR";
			root->left = LeftCHild;
			root->ivalue = tok1.tok;

			showTok();
			if(tok1.tok != Y_ASSIGN){
				printf("ERROR in stmt Y_ASSIGN\n");
				showTok();
				return NULL;
			}
			advance();
			past RightChild = newAstNode();
			RightChild = astExp();
			root->right = RightChild;
			printf("nodeType: %d\n", RightChild->nodeType);

			if(tok1.tok != Y_SEMICOLON){
				printf("ERROR in stmt Y_SEMICOLON\n");
				return NULL;
			}
			advance();

			return root;
		}
		case Y_LBRACKET:
		{
			//[Exp] ';'
			advance();
			past root = astExp();

			if(tok1.tok != Y_SEMICOLON){
				printf("ERROR in stmt\n");
				return NULL;
			}

			return root;
		}
		case Y_LSQUARE:
		{
			//Block
			root = astBlock();

			printf("\nbreak\n");
			return root;
		}
	}
}

// Number: IntConst
past astNumber()
{
	if (tok1.tok == num_INT)
	{
		past num = newNum(yylval.int_value);
		advance();
		return num;
	}
	return NULL;
}

/*
	60 Exp: AddExp
*/

past astExp()
{
	past root = astAddExp();
	return root;
}

/*
	61 LVal: Y_ID
	62     | Y_ID ArraySubscripts

	LVal → Ident {'[' Exp ']'}
*/
// same as CallExp!!!
past astLVal()
{
	if (tok1.tok == Y_ID)
	{
		past root = newAstNode();
		root->nodeType = DECL_REF_EXPR;
		root->nodeTypeStr = "DECL_REF_EXPR";
		root->svalue = tok1.my_yytext;
		advance();
		if (tok1.tok == Y_LBRACKET)
		{ // won't be conflicted with VarDef→Ident{'[' ConstExp ']'}
			advance(); // tok = EXP
			root->nodeType = ARRAY_SUBSCRIPT_EXPR; // change type to array
			root->nodeTypeStr = "ARRAY_SUBSCRIPT_EXPR";
			past ArrayName = newAstNode();
			ArrayName->nodeType = DECL_REF_EXPR;
			ArrayName->nodeTypeStr = "DECL_REF_EXPR";
			ArrayName->svalue = root->svalue;
			root->left = ArrayName;

			past ArraySubscript = astExp();
			root->right = ArraySubscript;

			if (tok1.tok == Y_RBRACKET)
			{
				advance();
			}
			else
			{
				printf("ERROR in astLVal!\n");
				return NULL;
			}
			while (tok1.tok == Y_LBRACKET)
			{
				printf("Y_LBRACKET\n");
				advance();
				past ArraySubscript2 = astExp();
				ArraySubscript->next = ArraySubscript2;
				if (tok1.tok == Y_RBRACKET)
				{
					advance();
				}
				else
				{
					printf("ERROR in astLVal!\n");
					return NULL;
				}
			}
		}
		return root;
	}
}

/*
	63 ArraySubscripts: Y_LSQUARE Exp Y_RSQUARE
	64                | Y_LSQUARE Exp Y_RSQUARE ArraySubscripts
*/

// past astArraySubscripts()
// {
//     if(tok1.tok == Y_LSQUARE)
//     {
//         advance();
//         if(Exp())
//         {
//             if(tok1.tok == Y_RSQUARE)
//             {
//                 advance();
//                 if(tok1.tok == Y_LSQUARE)
//                 {
//                     advance();
//                     return ArraySubscripts();
//                 }
//                 return 1;
//             }
//         }
//     }
//     return -1;
// }

/*
	65 PrimaryExp: Y_LPAR Exp Y_RPAR
	66           | LVal
	67           | num_INT
	68           | num_FLOAT
*/

past astPrimaryExp()
{
	if (tok1.tok == num_INT || tok1.tok == num_FLOAT)
	{
		past root = newAstNode();
		showTok();
		if (tok1.tok == num_INT)
		{
			root->nodeType = INTEGER_LITERAL;
			root->nodeTypeStr = "INTEGER_LITERAL";
			root->ivalue = atoi(tok1.my_yytext);
		}
		else
		{
			root->nodeType = FLOATING_LITERAL;
			root->nodeTypeStr = "FLOATING_LITERAL";
			root->ivalue = atof(tok1.my_yytext);
		}
		advance();
		return root;
	}
	else if (tok1.tok == Y_LPAR)
	{
		advance();
		past root = astExp();
		if (tok1.tok == Y_LPAR)
		{
			return root;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		past root = astLVal();
		return root;
	}
}

/*
    69 UnaryExp: PrimaryExp
    70         | Y_ID Y_LPAR Y_RPAR
    71         | Y_ID Y_LPAR CallParams Y_RPAR
    72         | Y_ADD UnaryExp
    73         | Y_SUB UnaryExp
    74         | Y_NOT UnaryExp
*/

past astUnaryExp()
{
	showPath();
	if (tok1.tok == Y_ID && tok2.tok == Y_LPAR)
	{
		// root node  : CallExpr
		// left node  : FuncName
		// right node : Parameters (connected throw next node)

		past root = newAstNode();
		past FuncName = newAstNode();

		FuncName->nodeType = DECL_REF_EXPR;
		root->nodeTypeStr = "DECL_REF_EXPR";
		FuncName->svalue = yytext;
		root->nodeType = CALL_EXPR;
		root->nodeTypeStr = "CALL_EXPR";
		root->left = FuncName;

		advance();
		if (tok1.tok == Y_LPAR)
		{
			showPath();
			advance();
			while (tok1.tok != Y_RPAR)
			{
				showPath();
				advance();
				past param = astExp();
				root->right = param;
				while (tok1.tok == Y_COMMA)
				{
					printf("\n A \n");
					showPath();
					advance();
					past param2 = astExp();
					param->next = param2;
					param2 = param;
				}
			}
			return root;
		}
		else
		{
			printf("EEROR in astUnaryExp!!!");
			return NULL;
		}
	}
	else if (tok1.tok == Y_ADD || tok1.tok == Y_SUB || tok1.tok == Y_NOT)
	{
		advance();
		past l = astUnaryExp();
		past p = newAstNode(); // create operator node
		p->left = l;
		p->nodeType = UNARY_OPERATOR;
		p->nodeTypeStr = "UNARY_OPERATOR";
		return p;
	}
	else
	{ // PrimaryExp
		past root = astPrimaryExp();
		return root;
	}
	// return false
}

int UnaryOp()
{
	if (tok1.tok == Y_ADD || tok1.tok == Y_SUB || tok1.tok == Y_NOT)
	{
		advance();
		return tok1.tok;
	}
	else
	{
		return -1;
	}
}

/*
    75 CallParams: Exp
    76           | Exp Y_COMMA CallParams

    FuncRParams → Exp { ',' Exp }
*/

// past CallParams()
// {
// 	past r = astExp();
// 	advance();
// 	while(tok1.tok == Y_COMMA)
// 	{
// 		advance();
// 		past l = astExp();

// 	}
// }

/*
    77 MulExp: UnaryExp
    78       | MulExp Y_MUL UnaryExp
    79       | MulExp Y_DIV UnaryExp
    80       | MulExp Y_MODULO UnaryExp
*/

past astMulExp()
{
	showPath();
	past root = astUnaryExp();
	while (tok1.tok == Y_MUL || tok1.tok == Y_DIV || tok1.tok == Y_MODULO)
	{
		int oper = tok1.tok;
		showYYTEXT();
		advance();
		past RightNode = astUnaryExp();
		root = newBinaryExp(oper, root, RightNode, NULL);
	}
	return root;
}

/*
    81 AddExp: MulExp
    82       | AddExp Y_ADD MulExp
    83       | AddExp Y_SUB MulExp
*/

past astAddExp()
{
	showPath();
	past root = astMulExp();
	while (tok1.tok == Y_ADD || tok1.tok == Y_SUB)
	{
		int oper = tok1.tok;
		showYYTEXT();
		advance();
		printf("%d\n", tok1.tok);
		past RightNode = astAddExp();
		root = newBinaryExp(oper, root, RightNode, NULL);
	}
	return root;
}

/*
    84 RelExp: AddExp
    85       | AddExp Y_LESS RelExp
    86       | AddExp Y_GREAT RelExp
    87       | AddExp Y_LESSEQ RelExp
    88       | AddExp Y_GREATEQ RelExp
*/

past astRelExp()
{
	showPath();
	past root = astAddExp();
	while (tok1.tok == Y_LESS || tok1.tok == Y_GREAT || tok1.tok == Y_LESSEQ || tok1.tok == Y_GREATEQ)
	{
		int oper = tok1.tok;
		showYYTEXT();
		advance();
		past RightNode = astAddExp();
		root = newBinaryExp(oper, root, RightNode, NULL);
	}
	return root;
}

/*
    89 EqExp: RelExp
    90      | RelExp Y_EQ EqExp
    91      | RelExp Y_NOTEQ EqExp
*/

past astEqExp()
{
	showPath();
	past root = astRelExp();
	while (tok1.tok == Y_EQ || tok1.tok == Y_NOTEQ)
	{
		int oper = tok1.tok;
		showYYTEXT();
		advance();
		past RightNode = astRelExp();
		root = newBinaryExp(oper, root, RightNode, NULL);
	}
	return root;
}
/*
    92 LAndExp: EqExp
    93        | EqExp Y_AND LAndExp
*/
past astLAndExp()
{
	showPath();
	past root = astEqExp();
	while (tok1.tok == Y_AND)
	{
		int oper = tok1.tok;
		showYYTEXT();
		advance();
		past RightNode = astEqExp();
		root = newBinaryExp(oper, root, RightNode, NULL);
	}
	return root;
}

/*
    94 LOrExp: LAndExp
    95       | LAndExp Y_OR LOrExp
*/

past astLOrExp()
{
	showPath();
	past root = astLAndExp();
	while (tok1.tok == Y_OR)
	{	// nodes connected with father and child (not next) .
		advance();
		int oper = tok1.tok;
		showYYTEXT();
		past RightNode = astLAndExp();
		// root->left = root;
		// root->right = astLAndExp();
		// root->svalue = tok1.my_yytext;
		// root->nodeType = BINARY_OPERATOR;
		root = newBinaryExp(oper, root, RightNode, NULL);
	}
	return root;
}

/*
    96 ConstExp: AddExp
*/

past astConstExp()
{
	showPath();
	past a = astAddExp();
	return a;
}

/*
    97 Type: Y_INT
    98     | Y_FLOAT
    99     | Y_VOID

*/

void showNode(past node, int layer)
{
	if(node == NULL)	return;
	for(int i=1; i<=layer; i++)
	{
		printf("|");
	}
	if(layer != 0) printf("-");
	printf("%-20s", node->nodeTypeStr);
	printf("layer: %3d ", layer);
	printf("token:<%7d , %7s>   \n", node->ivalue, node->svalue);
	if(node->nodeType == IF_STMT || node->nodeType == WHILE_STMT){
		showNode(node->if_cond, layer+1);
	}
	showNode(node->left, layer+1);
	showNode(node->right,layer+1);
	showNode(node->next, layer);    
}


void showTok(){
	printf("tok1:%d\n", tok1.tok);
	printf("tok2:%d\n", tok2.tok);
	printf("tok3:%d\n", tok3.tok);
}

int main(int argc, char **argv)
{
	{
		if (argc < 2)
			yyin = fopen("./expr.txt", "r");
		else
			yyin = fopen(argv[1], "r");
		path = 0;
		printf("input expression, 'q' to exit:\n");
		showTok();
		advance();advance();advance();
		past root = astCompUnit();
		showNode(root, 0);
		showTok();
		fclose(yyin);
	}
	return 0;
}