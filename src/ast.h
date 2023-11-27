#ifndef AST_H
#define AST_H

#include "symtable.h"
#include "lexer.h"

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
    FUNC_HEAD,// a is FUNC_HEAD_SIGNATURE & b is type
    FUNC_HEAD_SIGNATURE, // a is function name & b is *FuncDefArg

    VAR_DEF, // a VAR_TYPE ,b VAR_HEAD
    VAR_TYPE, // a Symbol_type , b bool nilable
    VAR_HEAD, // a name, b, EXPRESSION
    LET_DEF, // a VAR_TYPE, b VAR_HEAD
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

// Union literálů
union literalValue{
    int i;
    double d;
    char* s;
    Token t;
};

// Struktura uzlu výrazu
typedef struct{
    Token type;                 // typ uzlu
    union literalValue value;   // hodnota uzlu, v případě že jde o literál
    exp_node* left;             // levý potomek
    exp_node* right;            // pravý potomek
} exp_node;


FuncDefArg* FuncDefArg_new();

void FuncDefArg_free(FuncDefArg* arg);

FuncDefArg** Symbol_get_free_arg_p(Symbol* symbol);


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
