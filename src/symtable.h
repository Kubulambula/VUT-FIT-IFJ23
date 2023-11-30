#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include "error.h"
#include "ast.h"


#define SYMTABLE_INIT_SIZE 32
#define SYMTABLE_EXPAND_SIZE 16


typedef enum{
    FUNCTION, // func foo()
    VAR, // var foo
    LET, // let foo
} SymbolType;


typedef enum{
    NONE, //differentiate main and void func return type
    NIL, // void funkce
    INT,
    BOOL,   //need for expression return check
    DOUBLE,
    STRING,
} Type;




// typedef struct FuncDefArg{
//     char* name;
//     char* identifier;
//     Type type;
//     struct FuncDefArg* next;
// }FuncDefArg;


typedef struct{
    char* name;
    SymbolType symbol_type;
    Type type; // type of variable or return type of a function
    bool nilable; // if true, nil is allowed for this type
    bool initialized; // if true, the variable was assigned a value at least once
    FuncDefArg *args;  //list of arguments of function
} Symbol;


Symbol* Symbol_new();

void Symbol_free(Symbol* symbol);





typedef struct{
    Symbol** table;  // pointer to symbol table
    int size;   //table size
    int count;  //count of symbols inserted
    SymTable* previous;
} SymTable;


/**
 * Allocates space for array of pointers to symbols to default size of INIT_SIZE (30) 
 * Sets default values of symTable (.count , .size)
 * 
 * @param symTable Pointer to initialized SymTable
 * @returns true/false if the allocation was successfull 
*/
bool SymTable_init(SymTable* symTable);

/**
 * Deletes all symbols in table and frees the memory 
 * 
 * @param symTable Pointer to initialized SymTable
*/
void SymTable_free(SymTable* symTable);

/**
 * Inserts new Symbol to SymTable, if item allready exists, just sets new value
 * 
 * @param symTable Pointer to initialized SymTable
 * @param symbol Pointer to symbol 
 * @returns true/false if the insertion and allocation was successfull
*/
Error SymTable_insert(SymTable* symTable, Symbol* symbol);

/**
 * Returns a pointer to a matching symbol with a given name
 * 
 * @param symTable Pointer to initialized SymTable
 * @param name string with the name of searched symbol
 * @returns Symbol* to the matching symbol or NULL if matching symbol was not found
*/
//Symbol* Symtable_lookup(SymTable* symTable, char* name); // renamed as it was more clear
Symbol* SymTable_get(SymTable* symTable, char* name);


#endif