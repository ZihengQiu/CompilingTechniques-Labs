
#ifndef GENLLVM_H
#define GENLLVM_H

#include "ast.h"

enum
{
	T_INT = 1
};
#define true 1
#define false 0

int genExpr(past node);

int genArithmeticExpr(past node, char *result); //算术表达式
void genLogicExpr(past node, char *result, int trueNum, int falseNum);	//逻辑表达式
void genAssignExpr(past node, char *result);	// 赋值语句
void genIfStmt(past node, char *result);		// 条件语句
void genWhileStmt(past node, char *result);		// 循环语句
void genNode(past node, char *result);

void showCode(void);

#endif
