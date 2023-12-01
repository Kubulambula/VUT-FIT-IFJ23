#ifndef AST_H
#define AST_H

// #include "symtable.h"
#include "lexer.h"
#include "syntax_precedent.h"


typedef enum{
    TYPE_NONE, // for no return check
    TYPE_NIL, // void funkce
    TYPE_INT,
    //TYPE_BOOL,
    TYPE_DOUBLE,
    TYPE_STRING,
} Type;


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

    VAR_DEF, // a VAR_TYPE, b VAR_HEAD
    LET_DEF, // a VAR_TYPE, b VAR_HEAD
        VAR_TYPE, // a Type , b bool nilable
        VAR_HEAD, // a name, b, EXPRESSION
    
    ASSIGN, // a is variable name & b is EXPRESSION
    
    FUNC_CALL, // a is funcion name and b is FUNC_ARGS
        // >>>>> This could be reworked to be the same as FuncDefArg <<<<<
        FUNC_CALL_ARGS, // a is FUNC_ARG & b is FUNC_ARGS or NULL if this was the last arg
            FUNC_CALL_ARG, // a is name b is expression with value
    
    IFELSE, // a is expression condition, b IFELSE_BODIES
        IFELSE_BODIES, // a is true branch & b is false (else) branch. a & b are STATEMENTs
    WHILE, // a is expression condition and b is STATEMENT

    //CHECK_IF_LET // a is variable name
    // ^^^ o tomto se musime poradit na meetingu ^^^ - alternativy, jak to vyresit?
    
    RETURN, // a is expression - some operator & b is NULL

    EXPRESSION, // a is EXPRESSION_TREE, b is NULL 
    // ^^^ o tomto se musime poradit na meetingu ^^^ - je to potreba???
   
} ASTNodeType;


// One magical struct to rule them all
typedef struct{
    ASTNodeType type;
    void* a;  
    void* b;  
} ASTNode;


// // Union literalu
// union literalValue{
//     int i;
//     double d;
//     char* s;
//     Token t;
// };


// // Struktura uzlu výrazu
// typedef struct{
//     Token type;                 // typ uzlu
//     union literalValue value;   // hodnota uzlu, v případě že jde o literál
//     exp_node* left;             // levý potomek
//     exp_node* right;            // pravý potomek
// } exp_node;


FuncDefArg* FuncDefArg_new();

void FuncDefArg_free(FuncDefArg* arg);

// FuncDefArg** Symbol_get_free_arg_p(Symbol* symbol);
FuncDefArg** FuncDefArg_get_last_arg(FuncDefArg** arg);


ASTNode* ASTNode_new(ASTNodeType type);

void ASTNode_free(ASTNode* node);

ASTNode* ASTNode_find_leftmost_node(ASTNode* node);

ASTNode* ASTNode_find_rightmost_node(ASTNode* node);


#endif
