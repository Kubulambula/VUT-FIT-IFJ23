#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "ast.h"
#include "lexer.h"
#include "symtable.h"





Error funcCallCheck(ASTNode*func,Type* returnType,SymTable* tables,int scope);

Error handle_expression(exp_node* node,SymTable* tables,Type* returnType);
#endif