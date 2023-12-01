#ifndef SYNTAX_LL_H
#define SYNTAX_LL_H

#include "error.h"
#include "lexer.h"
#include "ast.h"


Error syntax(ASTNode** tree);

Error ll_program(BufferString* buffer_string, ASTNode** tree);

Error ll_func_definition(BufferString* buffer_string, ASTNode** tree);

Error ll_func_definition_head(BufferString* buffer_string, ASTNode** func_head);

Error ll_func_definition_head_args(BufferString* buffer_string, FuncDefArg** func_args);

Error ll_func_definition_head_arg(BufferString* buffer_string, FuncDefArg** func_arg);

Error ll_func_definition_body(BufferString* buffer_string, ASTNode** tree);

Error ll_statements(BufferString* buffer_string, ASTNode** tree);

Error ll_func_call(BufferString* buffer_string, ASTNode** tree, char* func_name);

Error ll_func_call_args(BufferString* buffer_string, ASTNode** tree);

Error ll_func_call_arg(BufferString* buffer_string, ASTNode** tree);

Error ll_func_call_arg_with_name(BufferString* buffer_string, ASTNode* tree);

Error ll_func_call_arg_without_name(BufferString* buffer_string, ASTNode* tree);

Error ll_assign(BufferString* buffer_string, ASTNode** tree, char* var_name);

Error ll_let_var_declaration(BufferString* buffer_string, ASTNode* tree);

Error ll_var_declaration(BufferString* buffer_string, ASTNode** tree);

Error ll_let_declaration(BufferString* buffer_string, ASTNode** tree);

Error ll_while(BufferString* buffer_string, ASTNode** tree);

Error ll_return(BufferString* buffer_string, ASTNode** tree);

Error ll_if(BufferString* buffer_string, ASTNode** tree);

Error ll_else(BufferString* buffer_string, ASTNode** tree);

Error generate_if_let_declaration_override_statement(ASTNode** tree, char* id);

#endif