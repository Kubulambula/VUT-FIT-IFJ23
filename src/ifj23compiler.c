#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "buffer_string.h"
#include "ast.h"
#include "lexer.h"
#include "syntax_ll.h"
#include "generator.h"


int main(void) {
	initLexer(stdin);
	
	Error err = OK;
	ASTNode* ast;

	err = syntax(&ast);
	if (err){
		fprintf(stderr, "Syntax ERR: %d\n", err);
		// syntax() frees the ast if it encounters an error automatically
		return err;
	}
	
	// No semantic yet :(
	// err = semantic(ast);
	// if (err){
	// 	fprintf(stderr, "Semantic ERR: %d\n", err);
	// 	ASTNode_free(ast);
	// 	return err;
	// }

	SymTable table;
	SymTable_init(&table);

	err = generate_code(ast, &table);
	if (err)
		fprintf(stderr, "Code generation ERR: %d\n", err);
	
	SymTable_free(&table);
	

	ASTNode_free(ast);
	if (err == OK)
		fprintf(stderr, "ERR: %d - good job!\n", err); // should be ok
	else
		fprintf(stderr, "ERR: %d\n", err);

	return OK;
}