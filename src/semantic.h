#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "ast.h"
#include "lexer.h"
#include "symtable.h"


Error semantic(ASTNode *code_tree,SymTable* codeTable);

Error appendScope(char**name,int scope);

Error funcCallCheck(ASTNode* func, Type* returnType, SymTable* tables, SymTable* codeTable, bool* nillable);

Error rearrange_global_statements(ASTNode* root, SymTable* codeTable, SymTable* globalTable,int*scope,bool recursive);

Error add_functions_to_symtable(ASTNode* root, SymTable* global_table, SymTable* code_table);

Error handle_expression(exp_node* node, SymTable* tables, Type* returnType, SymTable *code_table, int scoping, bool* nillable);

Error handle_statements(ASTNode* statement, SymTable* tables, SymTable* codeTable, Type expected_type, int* scope, bool* returned, bool nilable_return,bool main);
#endif