#ifndef CODE_H
#define CODE_H

#include "symtable.h"


typedef enum {
    ROOT, // the main big boy node. a is statement array with FUNC_DEF & b is statement array of main statements
    //EXPRESSION, // based on type, the a and b are operands or another expression subtree
    OPERATOR_PLUS,
    LITERAL_INT, // a is int
    LITERAL_DOUBLE, // a if double
    LITERAL_STRING, // a is char*
    FUNC_DEF, // a is name & b is llist of statements
    FUNC_CALL, // a is funcion name and b is FUNC_ARG list
    FUNC_ARG, // a is name b is expression with value
    RETURN, // a is expression - some operator
    IFELSE, // a is expression condition, b IFELSE_BODIES
    IFELSE_BODIES, // a is true branch & b is false (else) branch. a & b are statement arrays
    WHILE, // a is expression condition and b is brach (statement list)
    DEFVAR, // a is Symbol*
    ASSIGN, // a is symbol name and b is expression withthe value
} StatementType;


// One magical struct to rule them all
typedef struct bst_statement{
    StatementType type;
    struct bst_statement *a;  
    struct bst_statement *b;  
} bst_statement_t;

//list of trees?
typedef struct{
    struct bst_statement bst_Tree;
    LList* next;
} LList;

//TODO
//functions for structs, too tired from ISS hell
//void LList_dispose() for all trees ?? :C