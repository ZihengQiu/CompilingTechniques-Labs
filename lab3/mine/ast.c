#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"

int lines = 1;

past AstRoot = NULL;

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

past CreatRoot(void)
{
	past root = newAstNode();
	root->nodeTypeStr = "TranslationUnitDecl";
	return root;
}

past newBinaryExp(char* oper, past left, past right)
{
	past var = newAstNode();
	var->nodeTypeStr = "BINARY_OPERATOR";
	var->svalue = strdup(oper);
	var->left = left;
	var->right = right;
	return var;
}

past newBasicNode(char* NodeType, char* sVal, int iVal, past left, past right, past Next)
{
	past var = newAstNode();
	var->nodeTypeStr = NodeType;
	var->svalue = sVal;
	var->ivalue = iVal;
	var->left = left;
	var->right = right;
	var->next = Next;
	return var;
}

past newIfNode(past if_cond,past if_body,past else_body)
{
	past var = newAstNode();
	var->nodeTypeStr = "IF_STMT";
	var->if_cond = if_cond;
	var->left = if_body;
	var->right = else_body;
	return var;
}

void showNode(past node, int layer)
{
	if (node == NULL)
		return;
	for (int i = 1; i <= layer; i++)
	{
		printf("|");
	}
	if (layer != 0)
		printf("-");
	printf("%-20s", node->nodeTypeStr);
	printf("layer:%2d ", layer);
	printf("token:<%3d , %6s > ", node->ivalue, node->svalue);
	printf("\n");
	// printf("TypeValue:%10s\n", node->typeValue);

	if (node->nodeTypeStr == "IF_STMT" || node->nodeTypeStr == "WHILE_STMT")
	{
		showNode(node->if_cond, layer + 1);
	}
	showNode(node->left, layer + 1);
	showNode(node->right, layer + 1);
	showNode(node->next, layer);
}