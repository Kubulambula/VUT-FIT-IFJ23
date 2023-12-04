#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "ast.h"
#include "lexer.h"
#include "symtable.h"




Error appendScope(char*name,char**out,int scope,SymTable* tables);
Error funcCallCheck(ASTNode*func,Type* returnType,SymTable* tables,SymTable* codeTable,int scope);

Error handle_expression(exp_node* node,SymTable* tables,Type* returnType);
Error handle_statements(ASTNode*statement,SymTable* tables,SymTable*codeTable,Type expected_type,int scoping,bool* returned);
#endif