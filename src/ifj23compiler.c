#include <stdio.h>
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

	generate_code(ast);

	ASTNode_free(ast);

	fprintf(stderr, "ERR: %d - good job!\n", err); // should be ok
	return OK;
}