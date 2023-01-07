%{

#include <stdio.h>
#include <string.h>
#include "ast.h"

#define YYDEBUG 1

int yylex(void);
void yyerror(char *);

extern char *yytext;
extern past AstRoot;
extern int lines;

int n = 0;

void ShowYes()
{
	n++;
	printf("%d: Yes!\n",n);
}

void show(char* str)
{
	printf("%s \n", str);
}

%}

%union{
	int iValue;
	char* sValue;
	past pAst;
}

%token num_INT num_FLOAT INT FLOAT VOID CONST IF ELSE WHILE BREAK CONTINUE RETURN
%token ADD SUB MUL DIV MODULO LESS LESSEQ GREAT GREATEQ NOTEQ EQ NOT AND OR ASSIGN
%token LPAR RPAR LBRACKET RBRACKET LSQUARE RSQUARE COMMA SEMICOLON

%token <iValue> NUMBER
%token <sValue> ID

%type <pAst> ConstExp ConstExps AddExp LOrExp LAndExp EqExp RelExp MulExp UnaryExp PrimaryExp Exp
%type <pAst> CallParams LVal ArraySubscripts Block Stmt BlockItem BlockItems FuncFParam FuncFParams
%type <pAst> FuncDef InitVal InitVals VarDef VarDecls VarDecl ConstInitVals ConstInitVal ConstDef ConstDefs ConstDecl Decl
%type <pAst> CompUnit CompUnits Start

%% /* The grammar follows. */
// CompUnit: Decl CompUnit		{ show("CompUnit1:Decl CompUnit"); $$ = root==NULL ? CreatRoot() : $1; $$->next = $2; }
//         | FuncDef CompUnit	{ show("CompUnit2:FuncDef CompUnit"); $$ = root==NULL ? CreatRoot() : $1; $$->next = $2; }
//         | Decl				{ show("CompUnit3:Decl"); $$ = root==NULL ? CreatRoot() : $1; }
//         | FuncDef			{ show("CompUnit4:FuncDef"); $$ = root==NULL ? CreatRoot() : $1; }
// 		;
Start: CompUnits YYEOF			{ $$ = $1; AstRoot = $$; }
	;

CompUnits: CompUnit CompUnits	{ show("CompUnits1:CompUnit CompUnits"); $$ = $1; $$->next = $2; }
		| CompUnit				{ show("CompUnits2:CompUnit"); $$ = $1; }
		;

CompUnit: Decl				{ show("CompUnit1:Decl"); $$ = $1; }
		| FuncDef			{ show("CompUnit2:FuncDef"); $$ = $1; }
		;

Decl: ConstDecl	{ show("Decl:ConstDecl"); $$ = newBasicNode("DECL_STMT", NULL, -1, $1, NULL, NULL); }
    | VarDecl	{ show("Decl:VarDecl"); $$ = newBasicNode("DECL_STMT", NULL, -1, $1, NULL, NULL); }
	;

ConstDecl: CONST INT ConstDef SEMICOLON		{ $$ = $3; }
        | CONST INT ConstDefs SEMICOLON		{ $$ = $3; }
		;

ConstDefs: ConstDef COMMA ConstDef			{ $$ = $1; $$->next = $3; }
        | ConstDef COMMA ConstDefs			{ $$ = $1; $$->next = $3; }
		;

ConstDef: ID ASSIGN ConstInitVal			{ $$ = newBasicNode("VAR_DECL", $1, -1, $3, NULL, NULL); $$->typeValue = "const int"; }
        | ID ConstExps ASSIGN ConstInitVal	{ $$ = newBasicNode("VAR_DECL", $1, -1, $4, NULL, NULL); $$->typeValue = "const int"; }
		;

ConstExps: LSQUARE ConstExp RSQUARE				{ $$ = $2; }
        | LSQUARE ConstExp RSQUARE ConstExps	{ $$ = $2; $$->next = $4; }
		;

ConstInitVal: ConstExp										{ $$ = $1; }
            | LBRACKET RBRACKET								{ $$ = newBasicNode("INIT_LIST_EXPR", NULL, -1, NULL, NULL, NULL); }
            | LBRACKET ConstInitVal RBRACKET				{ $$ = newBasicNode("INIT_LIST_EXPR", NULL, -1, $2, NULL, NULL); }
            | LBRACKET ConstInitVal ConstInitVals RBRACKET	{ $$ = newBasicNode("INIT_LIST_EXPR", NULL, -1, $2, $3, NULL); }
			;

ConstInitVals: COMMA ConstInitVal				{ $$ = $2; }
            |COMMA ConstInitVal ConstInitVals	{ $$ = $2; $$->next = $3; }
			;

	//type : int or float
VarDecl: INT VarDef SEMICOLON				{ $$ = $2; $$->typeValue = "INT"; }
		| FLOAT VarDef SEMICOLON			{ $$ = $2; $$->typeValue = "FLOAT"; }
		| INT VarDef VarDecls SEMICOLON		{ $$ = $2; $$->typeValue = "INT"; $$->next = $3;}
		| FLOAT VarDef VarDecls SEMICOLON	{ $$ = $2; $$->typeValue = "FLOAT"; $$->next = $3;}
		;

VarDecls: COMMA VarDef					{ $$ = $2; }
        | COMMA VarDef VarDecls			{ $$ = $2; $$->next = $3; }
		;

VarDef: ID								{ $$ = newBasicNode("VAR_DECL", $1, -1, NULL, NULL, NULL); }
	| ID ASSIGN InitVal					{ $$ = newBasicNode("VAR_DECL", $1, -1, $3, NULL, NULL); }
	| ID ConstExps						{ $$ = newBasicNode("VAR_DECL", $1, -1, NULL, NULL, NULL); }
	| ID ConstExps ASSIGN InitVal		{ $$ = newBasicNode("VAR_DECL", $1, -1, $4, NULL, NULL); }
	;

InitVal: Exp									{ $$ = $1; }
		| LBRACKET RBRACKET						{ $$ = newBasicNode("INIT_LIST_EXPR", NULL, -1, NULL, NULL, NULL); }
		| LBRACKET InitVal RBRACKET				{ $$ = newBasicNode("INIT_LIST_EXPR", NULL, -1, $2, NULL, NULL); }
		| LBRACKET InitVal InitVals RBRACKET	{ $$ = newBasicNode("INIT_LIST_EXPR", NULL, -1, $2, $3, NULL); }
		;

InitVals: COMMA InitVal							{ $$ = $2; }
        | COMMA InitVal InitVals				{ $$ = $2; $$->next = $3; }
		;

FuncDef: INT ID LPAR RPAR Block					{ show("FuncDef 1"); $$ = newBasicNode("FUNCTION_DECL", $2, -1, NULL, $5, NULL); }
		| FLOAT ID LPAR RPAR Block				{ $$ = newBasicNode("FUNCTION_DECL", $2, -1, NULL, $5, NULL); }
		| INT ID LPAR FuncFParams RPAR Block	{ $$ = newBasicNode("FUNCTION_DECL", $2, -1, $4, $6, NULL); }
		| FLOAT ID LPAR FuncFParams RPAR Block	{ $$ = newBasicNode("FUNCTION_DECL", $2, -1, $4, $6, NULL); }
		;

FuncFParams: FuncFParam					{ $$ = $1; }
        | FuncFParam COMMA FuncFParams 	{ $$ = $1; $$->next = $3; }
		; // sequence of FuncParams changed

FuncFParam: INT ID									{ $$ = newBasicNode("PARM_DECL", $2, -1, NULL, NULL, NULL); }
        | INT ID LSQUARE RSQUARE					{ $$ = newBasicNode("PARM_DECL", $2, -1, NULL, NULL, NULL); }
        | INT ID ArraySubscripts					{ $$ = newBasicNode("PARM_DECL", $2, -1, NULL, NULL, NULL); }
        | INT ID LSQUARE RSQUARE ArraySubscripts	{ $$ = newBasicNode("PARM_DECL", $2, -1, NULL, NULL, NULL); }
		;

Block: LBRACKET BlockItems RBRACKET		{ $$ = newBasicNode("COMPOUND_STMT", NULL, -1, $2, NULL, NULL); }
    | LBRACKET RBRACKET					{ $$ = newBasicNode("COMPOUND_STMT", NULL, -1, NULL, NULL, NULL); }
	;

BlockItems: BlockItem			{ $$ = $1; }
        | BlockItem BlockItems	{ $$ = $1; $$->next = $2; }
		;

BlockItem: Decl					{ $$ = $1; }
        | Stmt					{ $$ = $1; }
		;

Stmt: LVal ASSIGN Exp SEMICOLON				{ $$ = newBasicNode("BINARY_OPERATOR", "=", -1, $1, $3, NULL); }	
    | Exp SEMICOLON							{ $$ = $1; }
    | Block									{ $$ = $1; }
    | WHILE LPAR LOrExp RPAR Stmt			{ $$ = newBasicNode("WHILE_STMT", NULL, -1, $3, $5, NULL); } 
    | IF LPAR LOrExp RPAR Stmt				{ $$ = newIfNode($3, $5, NULL);}
    | IF LPAR LOrExp RPAR Stmt ELSE Stmt	{ $$ = newIfNode($3, $5, $7);}
    | BREAK SEMICOLON						{ $$ = newBasicNode("BREAK_STMT", NULL, -1, NULL, NULL, NULL); }	
    | CONTINUE SEMICOLON					{ $$ = newBasicNode("CONTINUE_STMT", NULL, -1, NULL, NULL, NULL); }
    | RETURN Exp SEMICOLON					{ $$ = newBasicNode("RETURN_STMT", NULL, -1, $2, NULL, NULL); }
    | RETURN SEMICOLON						{ $$ = newBasicNode("RETURN_STMT", NULL, -1, NULL, NULL, NULL); }
	;

Exp: AddExp						{ show("Exp: AddExp"); $$ = $1; }
	;

LVal: ID						{ $$ = newBasicNode("DECL_REF_EXPR", $1, -1, NULL, NULL, NULL); }
    | ID ArraySubscripts		{ $$ = newBasicNode("ARRASUBSCRIPT_EXPR", $1, -1, $2, NULL, NULL); }
	;

ArraySubscripts: LSQUARE Exp RSQUARE			{ $$ = newBasicNode("DECL_REF_EXPR", NULL, -1, NULL, NULL, $2); }
		| ArraySubscripts LSQUARE Exp RSQUARE	{ $$ = newBasicNode("ARRASUBSCRIPT_EXPR", NULL, -1, $1, $3, NULL); }
		; // sequence of ArraySubscripts changed

PrimaryExp: LPAR Exp RPAR		{ $$ = $2; }
        | LVal					{ $$ = $1; }
        | num_INT				{ $$ = newBasicNode("INTEGER_LITERAL", NULL, yylval.iValue, NULL, NULL, NULL); }
        | num_FLOAT				{ $$ = newBasicNode("FLOAT_LITERAL", NULL, yylval.iValue, NULL, NULL, NULL); }
		;
		//remains to change


UnaryExp: PrimaryExp			{ $$ = $1; }
    | ID LPAR RPAR				{ $$ = newBasicNode("CALL_EXPR", $1, -1,newBasicNode("DECL_REF_EXPR", $1, -1, NULL, NULL, NULL), NULL, NULL); }
    | ID LPAR CallParams RPAR	{ $$ = newBasicNode("CALL_EXPR", $1, -1,newBasicNode("DECL_REF_EXPR", $1, -1, NULL, NULL, NULL), $3  , NULL); }
    | ADD UnaryExp				{ $$ = newBasicNode("UNARY_EXPR", "+", -1, NULL, NULL, $2); }
    | SUB UnaryExp				{ $$ = newBasicNode("UNARY_EXPR", "+", -1, NULL, NULL, $2); }
    | NOT UnaryExp				{ $$ = newBasicNode("UNARY_EXPR", "!", -1, NULL, NULL, $2); }
	;


CallParams: Exp					{ $$ = $1; }
        | Exp COMMA CallParams	{ $$ = $1; $$->next = $3; }
		;


MulExp: UnaryExp				{ $$ = $1;}
    | MulExp MUL UnaryExp		{ $$ = newBinaryExp("*", $1, $3); }
    | MulExp DIV UnaryExp		{ $$ = newBinaryExp("/", $1, $3); }
    | MulExp MODULO UnaryExp	{ $$ = newBinaryExp("%", $1, $3); }
	;

AddExp: MulExp
    | AddExp ADD MulExp			{ $$ = newBinaryExp("+", $1, $3); }
    | AddExp SUB MulExp			{ $$ = newBinaryExp("-", $1, $3); }
	;

RelExp: AddExp
    | RelExp LESS AddExp		{ $$ = newBinaryExp("<", $1, $3); }
    | RelExp GREAT AddExp		{ $$ = newBinaryExp(">", $1, $3); }
    | RelExp LESSEQ AddExp		{ $$ = newBinaryExp("<=", $1, $3); }
    | RelExp GREATEQ AddExp		{ $$ = newBinaryExp(">=", $1, $3); }
	;

EqExp: RelExp
	| EqExp EQ RelExp			{ $$ = newBinaryExp("==", $1, $3); }
	| EqExp NOTEQ RelExp		{ $$ = newBinaryExp("!=", $1, $3); }
	;

LAndExp: EqExp
	| LAndExp AND EqExp 		{ $$ = newBinaryExp("&&", $1, $3); }
	;

LOrExp:	LAndExp					
	| LOrExp OR LAndExp			{ $$ = newBinaryExp("||", $1, $3);}
	;


ConstExp: AddExp 				{ show("ConstExp: AddExp"); $$ = $1; }
	;

/* End of grammar. */
%%
void yyerror(char *s) 
{
	//fprintf(stderr, "%s\n", s);
	fprintf(stderr, "Error in line: %d Error: %s\n", lines, s);
}


