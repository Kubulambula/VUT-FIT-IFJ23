#include <stdio.h>

#include "error.h"
#include "lexer.h"
#include "buffer_string.h"
#include "syntax_ll.h"
#include "ast.h"



int main(void) {
	initLexer(stdin);
	
	Error err = OK;
	ASTNode* ast;

	err = syntax(&ast);
	if (err == OK)
		ASTNode_free(ast);

	printf("ERR: %d\n", err);
	return OK;
}