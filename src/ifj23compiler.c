#include <stdio.h>

#include "error.h"
#include "lexer.h"
#include "buffer_string.h"
#include "syntax_ll.h"
#include "ast.h"

int main(void) {
	
	initLexer(stdin);

	BufferString buffer_string;
	if (!BufferString_init(&buffer_string))
		return ERR_INTERNAL;
	
	SymTable table;
	if (!SymTable_init(&table))
		return ERR_INTERNAL;

	ASTNode* ast = ASTNode_new(ROOT);
	if (ast == NULL)
		return ERR_INTERNAL;

	Error err = ll_program(&buffer_string, &table, &ast);
	printf("ERR: %d\n", err);
	return err;
}