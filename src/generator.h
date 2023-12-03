#ifndef GENERATOR_H
#define GENERATOR_H

#include "error.h"
#include "ast.h"
#include "symtable.h"


char* format_string_to_IFJcode23_string(char* IFJ23_string);

char* get_generated_literal_int(int value);

char* get_generated_literal_double(double value);

char* get_generated_literal_string(char* value);

char* get_generated_literal_nil();

Error generate_func_call(ASTNode* func_call, SymTable* symtable);

Error generate_func_call_args(ASTNode* func_call_args, FuncDefArg* arg);

Error generate_func_call_arg(ASTNode* func_call_arg, FuncDefArg* arg);

Error generate_func_call_write_args(ASTNode* func_call_args, unsigned* args_on_stack);

Error generate_assing(ASTNode* assign, SymTable* symtable);

Error generate_user_functions(ASTNode* func_defs, SymTable* symtable);

Error generate_user_function(ASTNode* func_def, SymTable* symtable);

Error generate_user_function_comment_head(ASTNode* func_head);

Error generate_user_function_comment_head_signature(ASTNode* func_head_signature);

Error generate_main_body(ASTNode* statements, SymTable* symtable);

Error generate_code(ASTNode* root, SymTable* symtable);


#endif