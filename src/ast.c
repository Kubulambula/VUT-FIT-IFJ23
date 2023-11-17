#include <stdlib.h>
#include <assert.h>
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
