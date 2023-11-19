#ifndef AST_H
#define AST_H

#include "symtable.h"



typedef enum {
    // EXPRESSION = If it's a literal, the expression has the value of the literal.
    // If it's an operator, the value must be computed first from arguments. (true expression)
    ROOT, // The main big boy node. a is LList with FUNC_DEFs & b is LList of main statements
    STATEMENT, // a is the statement & b is next statement or NULL if this is the last statement
    FUNC_DEFS, // a is FUNC_DEF & b is FUNC_DEFS or NULL if no more functions are defined
    FUNC_DEF, // a is function name & b is LList of function statements
    VAR_DEF, // a is variable name & b is OPTIONAL value. If Symbol.type is NIL then it must be inferred
    LET_DEF, // a is variable name & b is OPTIONAL value. If Symbol.type is NIL then it must be inferred
    
    ASSIGN, // a is variable name & b is EXPRESSION


    //EXPRESSION, // based on type, the a and b are operands or another expression subtree
    OPERATOR_PLUS,
    LITERAL_INT, // a is int
    LITERAL_DOUBLE, // a if double
    LITERAL_STRING, // a is char*
    FUNC_CALL, // a is funcion name and b is FUNC_ARG list
    FUNC_ARG, // a is name b is expression with value
    RETURN, // a is expression - some operator
    IFELSE, // a is expression condition, b IFELSE_BODIES
    IFELSE_BODIES, // a is true branch & b is false (else) branch. a & b are statement arrays
    WHILE, // a is expression condition and b is brach (statement list)
} ASTNodeType;


// One magical struct to rule them all
typedef struct{
    ASTNodeType type;
    void* a;  
    void* b;  
} ASTNode;


typedef struct LList{
    void* value;
    struct LList* next;
} LList;



ASTNode* ASTNode_new(ASTNodeType type);

ASTNode* ASTNode_find_leftmost_node(ASTNode* node);


//TODO
//functions for structs, too tired from ISS hell
//void LList_dispose() for all trees ?? :C
#endif
