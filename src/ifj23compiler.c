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
	
	ASTNode* ast;
    SymTable *symtable;

	ERR = syntax(&ast);
	if (ERR){
		// syntax() frees the ast if it encounters an error automatically
		return ERR;
	}

	symtable = (SymTable*)malloc(sizeof(SymTable));
    if(symtable == NULL){
		ASTNode_free(ast);
        return ERR_INTERNAL;
    }
	if (!SymTable_init(symtable)){
		ASTNode_free(ast);
		free(symtable);
		return ERR_INTERNAL;
	}

	ERR = semantic(ast, symtable);
	if (ERR){
		ASTNode_free(ast);
		SymTable_free(symtable);
		// free(symtable) // SymTable_free() frees the pointer as well
		return ERR;
	}

	ERR = generate_code(ast, symtable);
	
	SymTable_free(symtable);
	ASTNode_free(ast);

	return ERR;
}