#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "genllvm.h"

char sprintfBuffer[500];	// buffer for sprintf. usually saves oen single instruction.
char codeSaver[50005];		// saves the generated code.

int temVarNum = 0;			// the number of temporary variables.

int breakNum[100];			// the number of break statements.
int breakNumCount = 0;		

void addLLVMCodes(char *codes)
{
	strcat(codeSaver, codes);
	// printf("%s", codes);
}

void showLabel(int lable)		// show the label.
{
	// printf("\n%d\n", lable);
	sprintf(sprintfBuffer, "\n%d\n", lable);
	addLLVMCodes(sprintfBuffer);
}

int getTemVarNum()
{
	temVarNum++;
	return temVarNum;
}

int genPrimaryExpr(past node, char *operand)
{
	int type = -1;

	if (strcmp(node->nodeTypeStr, "INTEGER_LITERAL") == 0)
	{
		type = T_INT;
		// if (operand != NULL) // operand is originally not NULL
		// {
		// 	sprintf(operand, "%d", node->ivalue);
		// }
		sprintf(operand, "%d", node->ivalue);
	}
	else if (strcmp(node->nodeTypeStr, "BINARY_OPERATOR") == 0 || strcmp(node->nodeTypeStr, "UNARY_EXPR") == 0)
	{
		// 表达式的中间结果用临时变量保存
		// 其结果为当前的 temVarNum
		type = genArithmeticExpr(node, NULL);
		if (operand != NULL)
			sprintf(operand, "%%%d", temVarNum);
	}
	else
	{
		printf("ERROR: 发现不支持的运算类型\n");
	}
	return type;
}

int genArithmeticExpr(past node, char *result)
{
	char loperand[50];
	char roperand[50];
	char oper[10];

	int ltype = -1;
	int rtype = -1;

	if (strcmp(node->nodeTypeStr, "UNARY_EXPR") == 0)// 一元表达式左操作符为空
	{
		rtype = genPrimaryExpr(node->next, roperand);
		ltype = rtype;
		sprintf(loperand, "0");
	}
	else if (strcmp(node->nodeTypeStr, "BINARY_OPERATOR") == 0)
	{
		ltype = genPrimaryExpr(node->left, loperand);
		rtype = genPrimaryExpr(node->right, roperand);
	}
	else if(strcmp(node->nodeTypeStr, "DECL_REF_EXPR") == 0)
	{
		// printf("  %%%d = load i32, i32* %%%s, align 4\n", getTemVarNum(), node->svalue);
		sprintf(sprintfBuffer, "  %%%d = load i32, i32* %%%s, align 4\n", getTemVarNum(), node->svalue);
		addLLVMCodes(sprintfBuffer);

		sprintf(result, "%%%d", temVarNum);
		return temVarNum;
	}
	else if (strcmp(node->nodeTypeStr, "INTEGER_LITERAL") == 0)
	{
		sprintf(result, "%d", node->ivalue);
	}

	if (ltype == rtype && ltype == T_INT)
	{

		switch ((int)node->svalue[0])
		{
		case '+':
			sprintf(oper, "add nsw");
			break;
		case '-':
			sprintf(oper, "sub nsw");
			break;
		case '*':
			sprintf(oper, "mul nsw");
			break;
		case '/':
			sprintf(oper, "sdiv");
			break;
		case '%':
			sprintf(oper, "srem");
			break;
		}

		sprintf(sprintfBuffer, "  %%%d = %s i32 %s, %s\n", getTemVarNum(), oper, loperand, roperand);
		addLLVMCodes(sprintfBuffer);

		return T_INT;
	}
}

void genVarDecl(past node, char* result)
{
	// 生成变量声明
	// 生成的指令为： %varName = alloca i32, align 4
	char loperand[50];

	sprintf(sprintfBuffer, "  %%%s = alloca i32, align 4\n", node->svalue);
	// printf("%s", sprintfBuffer);
	addLLVMCodes(sprintfBuffer);
	if(node->left != NULL)
	{
		genArithmeticExpr(node->left, loperand);
		// printf("  store i32 %s, i32* %%%s, align 4\n", loperand, node->svalue);
		sprintf(sprintfBuffer, "  store i32 %s, i32* %%%s, align 4\n", loperand, node->svalue);
		addLLVMCodes(sprintfBuffer);
	}
}

void genReturnStmt(past node, char* result)
{
	// 生成返回语句
	// 生成的指令为： ret i32 %varName
	if(node->left == NULL)
	{
		// printf("  ret void\n");
		sprintf(sprintfBuffer, "  ret void\n");
		addLLVMCodes(sprintfBuffer);
		return ;
	}
	char loperand[50];
	genArithmeticExpr(node->left, loperand);
	sprintf(sprintfBuffer, "  ret i32 %s\n", loperand);
	// printf("%s\n", sprintfBuffer);
	addLLVMCodes(sprintfBuffer);
	return;
}

void genBreakStmt(past node, char* result)
{
	// 生成break语句
	// 生成的指令为： br label %break

	sprintf(sprintfBuffer, "  br label %%%d\n", breakNum[breakNumCount--]);
	// printf("%s\n", sprintfBuffer);
	addLLVMCodes(sprintfBuffer);
}

void genAssignExpr(past node, char *result)
{
	//store i32 Val, i32* %IDnum, align 4
	int IDnum = 1, Val = 0;
	Val = node->right->ivalue;
	// int IDnum = SearchID(node->left->svalue); //LVal

	sprintf(sprintfBuffer, "  store i32 %d, i32* %%%d, align 4\n", Val, IDnum);
	addLLVMCodes(sprintfBuffer);
}

void genLogicExpr(past node, char *result, int trueNum, int falseNum)
{
	// %1 = icmp slt i32 %0, 10
	// %2 = icmp sgt i32 %0, 0
	// %3 = and i1 %1, %2
	// br i1 %3, label %if.then, label %if.else
	//<result> = icmp <cond> <ty> <op1>, <op2>
	char loperand[50];
	char roperand[50];
	char cond[10];

	if(strcmp(node->svalue, "&&") == 0){
		int andNum = getTemVarNum();
		genLogicExpr(node->left, loperand, andNum, falseNum);
		showLabel(andNum);
		genLogicExpr(node->left, roperand, trueNum, falseNum);
		return;
	}
	else if(strcmp(node->svalue, "||") == 0){
		int orNum = getTemVarNum();
		genLogicExpr(node->left, loperand, trueNum, orNum);
		showLabel(orNum);
		genLogicExpr(node->left, roperand, trueNum, falseNum);
		return;
	}

	genArithmeticExpr(node->left, loperand);
	genArithmeticExpr(node->right, roperand);

	if(strcmp(node->svalue, ">") == 0){
		strcpy(cond, "sgt");
	}
	else if(strcmp(node->svalue, ">=") == 0){
		strcpy(cond, "sge");
	}
	else if(strcmp(node->svalue, "<") == 0){
		strcpy(cond, "slt");
	}
	else if(strcmp(node->svalue, "<=") == 0){
		strcpy(cond, "sle");
	}
	else if(strcmp(node->svalue, "==") == 0){
		strcpy(cond, "eq");
	}
	else if(strcmp(node->svalue, "!=") == 0){
		strcpy(cond, "ne");
	}
	else{
		printf("ERROR: 发现不支持的运算类型\n");
	}

	int condNum = getTemVarNum();
	//<result> = icmp <cond> <ty> <op1>, <op2>
	// printf("  %%%d = icmp %s i32 %s, %s\n", condNum, cond, loperand, roperand);
	sprintf(sprintfBuffer, "  %%%d = icmp %s i32 %s, %s\n", condNum, cond, loperand, roperand);
	addLLVMCodes(sprintfBuffer);

	// printf("  br i1 %%%d, label %%%d, label %%%d\n", condNum, trueNum, falseNum);
	sprintf(sprintfBuffer, "  br i1 %%%d, label %%%d, label %%%d\n", condNum, trueNum, falseNum);
	addLLVMCodes(sprintfBuffer);
}

// IF LPAR LOrExp RPAR Stmt				{ $$ = newIfNode($3, $5, NULL)
// IF LPAR LOrExp RPAR Stmt ELSE Stmt 	{ $$ = newIfNode($3, $5, $7); }	//ifCond+left+right
void genIfStmt(past node, char *result)
{
	if(node == NULL) return;

	char loperand[50];
	char roperand[50];
	int trueNum = getTemVarNum(), falseNum = getTemVarNum();

	genLogicExpr(node->if_cond, result, trueNum, falseNum);
	showLabel(trueNum);
	genNode(node->left, result);
	// genNode(node->next, result);

	if(node->right == NULL) // without else
	{
		// printf("  br label %%%d\n", falseNum);
		sprintf(sprintfBuffer, "  br label %%%d\n", falseNum);
		addLLVMCodes(sprintfBuffer);
		showLabel(falseNum);
	}else{	// have else
		int elseNum = getTemVarNum();
		// printf("  br label %%%d\n", elseNum);
		sprintf(sprintfBuffer, "  br label %%%d\n", elseNum);
		addLLVMCodes(sprintfBuffer);
		showLabel(falseNum);
		genNode(node->right, result);
		// printf("  br label %%%d\n", elseNum);
		sprintf(sprintfBuffer, "  br label %%%d\n", elseNum);
		addLLVMCodes(sprintfBuffer);
		showLabel(elseNum);
	}
}

// WHILE LPAR LOrExp RPAR Stmt			{ $$ = newBasicNode("WHILE_STMT", NULL, -1, $3, $5, NULL); }
void genWhileStmt(past node, char *result)
{
	char loperand[50];
	char roperand[50];
	int condNum = getTemVarNum(), trueNum = getTemVarNum(), falseNum = getTemVarNum();
	breakNum[++breakNumCount] = falseNum;
	// printf("  br label %%%d\n", condNum);
	sprintf(sprintfBuffer, "  br label %%%d\n", condNum);
	addLLVMCodes(sprintfBuffer);

	showLabel(condNum);
	genLogicExpr(node->left, loperand, trueNum, falseNum);

	showLabel(trueNum);
	genNode(node->right, result);
	// printf("  br label %%%d\n", condNum);
	sprintf(sprintfBuffer, "  br label %%%d\n", condNum);
	addLLVMCodes(sprintfBuffer);

	showLabel(falseNum);
}

void genNode(past node, char* result)
{
	if (node == NULL)
		return;

	if (strcmp(node->nodeTypeStr, "BINARY_OPERATOR") == 0 && strcmp(node->svalue, "=") == 0)
	{
		genAssignExpr(node, result);
	}
	else if (strcmp(node->nodeTypeStr, "BINARY_OPERATOR") == 0 || strcmp(node->nodeTypeStr, "UNARY_EXPR") == 0)
	{
		genArithmeticExpr(node, result);
	}
	else if(strcmp(node->nodeTypeStr, "IF_STMT") == 0)
	{
		genIfStmt(node, result);
	}
	else if (strcmp(node->nodeTypeStr, "WHILE_STMT") == 0)
	{
		genWhileStmt(node, result);
	}
	else if (strcmp(node->nodeTypeStr, "VAR_DECL") == 0)
	{
		genVarDecl(node, result);
	}
	else if (strcmp(node->nodeTypeStr, "RETURN_STMT") == 0)
	{
		genReturnStmt(node, result);
	}
	else if (strcmp(node->nodeTypeStr, "BREAK_STMT") == 0)
	{
		genBreakStmt(node, result);
	}
	else
	{
		genNode(node->left, result);
		genNode(node->right, result);
	}
	genNode(node->next, result);

	return ;
}

void showCode(void)
{
	//print codeSaver[]
	for(int i = 0; i < strlen(codeSaver); i++)
	{
		printf("%c", codeSaver[i]);
	}
}