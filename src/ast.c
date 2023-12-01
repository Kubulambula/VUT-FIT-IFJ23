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

void ASTNode_free(ASTNode* node){
    if (node == NULL)
        return;
    
    switch (node->type){
        case ROOT:
            ASTNode_free((ASTNode*)node->a);
            ASTNode_free((ASTNode*)node->b);
            free(node);
            break;
        
        case STATEMENT:
            free((ASTNode*)node->a); // free the specific statement (assign, if, while, ...)
            free((ASTNode*)node->b); // free the next statement
            free(node);
            break;
        
        case FUNC_DEFS:
            ASTNode_free((ASTNode*)node->a);
            ASTNode_free((ASTNode*)node->b);
            free(node);
            break;
        
        case FUNC_DEF:
            ASTNode_free((ASTNode*)node->a);
            ASTNode_free((ASTNode*)node->b);
            free(node);
            break;
        
        case FUNC_HEAD:
            ASTNode_free((ASTNode*)node->a);
            // node->b is Type - no free needed
            free(node);
            break;
        
        case FUNC_HEAD_SIGNATURE:
            free(node->a); // free function name
            FuncDefArg_free((FuncDefArg*)(node->b)); // free FuncDefArgs
            free(node);
            break;
        
        case VAR_DEF:
        case LET_DEF:
            ASTNode_free((ASTNode*)node->a); // free VAR_TYPE
            ASTNode_free((ASTNode*)node->b); // free VAR_HEAD
            free(node);
            break;
        
        case VAR_TYPE:
            // do nothing - no need to free anything
            free(node);
            break;
        
        case VAR_HEAD:
            free(node->a); // free the var name
            // TODO free the expression
            free(node);
            break;
        
        case ASSIGN:
            free(node->a); // free the var name
            // TODO free the expression
            free(node);
            break;
        
        case FUNC_CALL:
            free(node->a); // free func name
            ASTNode_free((ASTNode*)node->b); // free the args
            free(node);
            break;
        
        case FUNC_CALL_ARGS:
            ASTNode_free((ASTNode*)node->a); // free FUNC_ARG
            ASTNode_free((ASTNode*)node->b); // free FUNC_ARGS
            free(node);
            break;
        
        case FUNC_CALL_ARG:
            free(node->a); // free the arg name
            // TODO free the expression
            free(node);
            break;
        
        case IFELSE:
            // TODO free the expression
            ASTNode_free((ASTNode*)node->b); // free the bodies
            free(node);
            break;
        
        case IFELSE_BODIES:
            ASTNode_free((ASTNode*)node->a); // free the true branch
            ASTNode_free((ASTNode*)node->b); // free the false branch
            free(node);
            break;
        
        case WHILE:
            // TODO free the expression
            ASTNode_free((ASTNode*)node->b); // free the while body
            free(node);
            break;
        
        case RETURN:
            // TODO free the expression
            free(node);
            break;
        
        case EXPRESSION:
            break;
        
        default:
            break;
    }
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
