#ifndef AST_H
#define AST_H

#include "symtable.h"

typedef struct FuncDefArg{
    char* name;
    char* identifier;
    Type type;
    struct FuncDefArg* next;
}FuncDefArg;

typedef enum {
    // EXPRESSION = If it's a literal, the expression has the value of the literal.
    // If it's an operator, the value must be computed first from arguments. (true expression)
    ROOT, // The main big boy node. a is FUNC_DEFs & b is STATEMENT of main
    STATEMENT, // a is the statement & b is next statement or NULL if this is the last statement
    FUNC_DEFS, // a is FUNC_DEFS or NULL , b FUNC_DEF
    FUNC_DEF, // a is FUNC_HEAD & b is STATEMENT
    FUNC_HEAD,// a is func name & b is *FuncDefArg

    VAR_DEF, // a is variable name & b is OPTIONAL value. If Symbol.type is NIL then it must be inferred
    LET_DEF, // a is variable name & b is OPTIONAL value. If Symbol.type is NIL then it must be inferred
    ASSIGN, // a is variable name & b is EXPRESSION
    FUNC_CALL, // a is funcion name and b is FUNC_ARG list
        FUNC_CALL_ARG, // a is name b is expression with value
    IFELSE, // a is expression condition, b IFELSE_BODIES
        IFELSE_BODIES, // a is true branch & b is false (else) branch. a & b are statement arrays
    WHILE, // a is expression condition and b is brach (statement list)
    RETURN, // a is expression - some operator


 
    EXPRESSION, // a is EXPRESSION_TREE, b is NULL 
   
} ASTNodeType;


// One magical struct to rule them all
typedef struct{
    ASTNodeType type;
    void* a;  
    void* b;  
} ASTNode;


FuncDefArg* FuncDefArg_new();

void FuncDefArg_free(FuncDefArg* arg);

// typedef struct LList{
//     void* value;
//     struct LList* next;
// } LList;





ASTNode* ASTNode_new(ASTNodeType type);

ASTNode* ASTNode_find_leftmost_node(ASTNode* node);


//TODO
//functions for structs, too tired from ISS hell
//void LList_dispose() for all trees ?? :C
#endif
