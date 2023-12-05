#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "buffer_string.h"
#include "ast.h"
#include "semantic.h"

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

    printf("syntax ok\n");

    SymTable *symtable = (SymTable*)malloc(sizeof(SymTable));
    if(symtable == NULL){
		ASTNode_free(ast);
        return ERR_INTERNAL;
    }
	if (!SymTable_init(symtable)){
		ASTNode_free(ast);
		free(symtable);
		return ERR_INTERNAL;
	}

    printf("symtable ok\n");


	err = semantic(ast, symtable);
	if (err){
		fprintf(stderr, "Semantic ERR: %d\n", err);
		ASTNode_free(ast);
		SymTable_free(symtable);
		return err;
	}

    printf("semantic ok\n");


	err = generate_code(ast, symtable);
	if (err)
		fprintf(stderr, "Code generation ERR: %d\n", err);
	
	
    printf("codegen ok\n");
    print_ast_node(ast, 0);

	SymTable_free(symtable);
	

	ASTNode_free(ast);
	if (err == OK)
		fprintf(stderr, "ERR: %d - good job!\n", err); // should be ok
	else
		fprintf(stderr, "ERR: %d\n", err);

	return OK;
}