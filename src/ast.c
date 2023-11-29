#include <stdlib.h>
// #include <assert.h>
#include "ast.h"


ASTNode* ASTNode_new(ASTNodeType type){
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	if (node != NULL){
		node->type = type;
		node->a = NULL;
		node->b = NULL;
	}
	return node;
}


ASTNode* ASTNode_find_leftmost_node(ASTNode* node){
	if(node == NULL)
		return NULL;
	
	while (node->a != NULL){
		node = node->a;
	}

	return node;
}


ASTNode* ASTNode_find_rightmost_node(ASTNode* node){
	if(node == NULL)
		return NULL;
	
	while (node->b != NULL){
		node = node->b;
	}

	return node;
}


FuncDefArg* FuncDefArg_new(){
    FuncDefArg* arg = (FuncDefArg*)malloc(sizeof(FuncDefArg));
    if (arg != NULL){
        arg->name = NULL;
        arg->identifier = NULL;
        arg->type = TYPE_NIL;
        arg->next = NULL;
    }

    return arg;
}


void FuncDefArg_free(FuncDefArg* arg){
    if (arg == NULL)
        return;
    
    if (arg->name != NULL)
        free(arg->name);
    if (arg->identifier != NULL)
        free(arg->identifier);
    if (arg->next != NULL)
        FuncDefArg_free(arg->next);
    free(arg);
}


FuncDefArg** FuncDefArg_get_last_arg(FuncDefArg** arg){
    if (*arg == NULL)
        return arg;
    
    while((*arg)->next != NULL)
        arg = &((*arg)->next);
    
    return arg;
}
