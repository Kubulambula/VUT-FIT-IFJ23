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
            ASTNode_free((ASTNode*)node->a); // free the specific statement (assign, if, while, ...)
            ASTNode_free((ASTNode*)node->b); // free the next statement
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
            ASTNode_free((ASTNode*)node->b);
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
            exp_node_purge(node->b); // free the expression
            free(node);
            break;
        
        case ASSIGN:
            free(node->a); // free the var name
            exp_node_purge(node->b); // free the expression
            free(node);
            break;
        
        case FUNC_CALL:
            // printf(">>>%s\n",);
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
            exp_node_purge(node->b); // free the expression
            free(node);
            break;
        
        case IFELSE:
            exp_node_purge(node->a); // free the expression
            ASTNode_free((ASTNode*)node->b); // free the bodies
            free(node);
            break;
        
        case IFELSE_BODIES:
            ASTNode_free((ASTNode*)node->a); // free the true branch
            ASTNode_free((ASTNode*)node->b); // free the false branch
            free(node);
            break;
        
        case WHILE:
            exp_node_purge(node->a); // free the expression
            ASTNode_free((ASTNode*)node->b); // free the while body
            free(node);
            break;
        
        case RETURN:
            exp_node_purge(node->b); // free the expression
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
        arg->nilable = false;
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


// FuncDefArg* FuncDefArg_get_last_arg(FuncDefArg* arg){
//     if (arg == NULL)
//         return NULL;
    
//     while(arg->next != NULL)
//         arg = arg->next;
    
    //return arg;
//}

// offset nastavte pri prvnim volani na 0
void print_tree_string(char* string, int offset){
    for(int i = 0; i < offset; i++){
            printf("|   ");
        }
        printf("%s\n", string);
}

// offset nastavte pri prvnim volani na 0
void print_ast_node(ASTNode *node, int offset){
    if(node != NULL){
        for(int i = 0; i < offset; i++){
            printf("|   ");
        }
        printf("( ");
        print_astnode_as_string(node->type);
        printf(" )\n");


        switch(node->type){
            case FUNC_HEAD_SIGNATURE:
            case VAR_TYPE:
                break;
            case FUNC_CALL:
                print_tree_string((char*)node->a,offset+1);
                print_ast_node(node->b, offset+1);
                break;
            case VAR_HEAD:
            case ASSIGN:
            case FUNC_CALL_ARG:
                print_tree_string((char*)node->a,offset+1);
                print_exp_node((exp_node*)node->b, offset+1);
                break;

            case IFELSE:
            case WHILE:
                print_exp_node((exp_node*)node->a, offset+1);
                print_ast_node(node->b, offset+1);
                break;

            case EXPRESSION:
                print_exp_node((exp_node*)node->a, offset+1);
                break;

            default:
                print_ast_node(node->a, offset+1);
                print_ast_node(node->b, offset+1);
        }
    }
}

// offset nastavte pri prvnim volani na 0
void print_exp_node(exp_node *node, int offset){
    if(node != NULL){
        for(int i = 0; i < offset; i++){
            printf("|   ");
        }
        printf("[ ");
        print_token_as_string(node->type);
        printf(" ]");
        switch(node->type){
            case TOKEN_LITERAL_INT:
                printf(" %d", node->value.i);
                break;
            case TOKEN_LITERAL_DOUBLE:
                printf(" %lf", node->value.d);
                break;
            case TOKEN_LITERAL_STRING:
            case TOKEN_IDENTIFIER:
            case TOKEN_KEYWORD_FUNC:
                printf(" %s", node->value.s);
                break;
            default:
                break;
        }
        printf("\n");
        if(node->type == TOKEN_KEYWORD_FUNC){
            print_ast_node((ASTNode*)node->left, offset+1);
        }
        else{
            print_exp_node(node->left, offset+1);
            print_exp_node(node->right, offset+1);
        }
    }
}
