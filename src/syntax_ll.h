#ifndef SYNTAX_LL_H
#define SYNTAX_LL_H

//#include <stdbool.h>
//#include "buffer_string.h"
//#include "syntax_precedent.h"
#include "error.h"
#include "lexer.h"
#include "ast.h"


#define GET_TOKEN(skip_eol) \
	CURRENT_TOKEN = get_token(buffer_string, skip_eol); \
	if (CURRENT_TOKEN == TOKEN_ERR_INTERNAL || CURRENT_TOKEN == TOKEN_ERR_LEXICAL) \
		return (CURRENT_TOKEN == TOKEN_ERR_INTERNAL ? ERR_INTERNAL : ERR_LEXICAL); \


Error ll_program(BufferString* buffer_string, SymTable* table, ASTNode** tree);

Error ll_func_definition(BufferString* buffer_string, SymTable* table, ASTNode** tree);

Error ll_func_definition_head(BufferString* buffer_string, SymTable* table, ASTNode** tree);

Error ll_func_definition_head_args(BufferString* buffer_string, SymTable* table, ASTNode** tree);

Error ll_func_definition_head_arg(BufferString* buffer_string, SymTable* table, ASTNode** tree);

Error ll_func_definition_body(BufferString* buffer_string, SymTable* table, ASTNode** tree);

Error ll_func_statements(BufferString* buffer_string, void* tree);

Error ll_statements(BufferString* buffer_string, void* tree);

Error ll_func_call(BufferString* buffer_string, void* tree);

Error ll_func_call_args(BufferString* buffer_string, void* tree);

Error ll_func_call_arg(BufferString* buffer_string, void* tree);

Error ll_func_call_arg_with_name(BufferString* buffer_string, void* tree);

Error ll_func_call_arg_without_name(BufferString* buffer_string, void* tree);

Error ll_assign(BufferString* buffer_string, void* tree);

Error ll_var_declaration(BufferString* buffer_string, void* tree);

Error ll_let_declaration(BufferString* buffer_string, void* tree);

Error ll_while(BufferString* buffer_string, void* tree);

Error ll_return(BufferString* buffer_string, void* tree);

Error ll_if(BufferString* buffer_string, void* tree);

#endif