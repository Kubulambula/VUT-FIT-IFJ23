#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>


#define SYMTABLE_INIT_SIZE 32
#define SYMTABLE_EXPAND_SIZE 16


typedef enum{
    FUNCTION, // func foo()
    VAR_MUTABLE, // var foo
    VAR_IMMUTABLE, // let foo
} SymbolType;


typedef enum{
    NIL, // void funkce
    INT,
    //BOOL,
    DOUBLE,
    STRING,
} Type;


typedef struct{
    char* name;
    SymbolType symbol_type;
    Type type; // type of variable or return type of a function
    bool nilable; // if true, nil is allowed for this type
    bool initialized; // if true, the variable was assigned a value at least once
} Symbol;


typedef struct{
    Symbol** table;  // pointer to symbol table
    int size;   //table size
    int count;  //count of symbols inserted
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
 * Frees the symbol
 * 
 * @param symbol Pointer to initialized symbol
*/
void Symbol_free(Symbol* symbol);

/**
 * Inserts new Symbol to SymTable, if item allready exists, just sets new value
 * 
 * @param symTable Pointer to initialized SymTable
 * @param symbol Pointer to symbol 
 * @returns true/false if the insertion and allocation was successfull
*/
bool SymTable_insert(SymTable* symTable, Symbol* symbol);

/**
 * Returns a pointer to a matching symbol with a given name
 * 
 * @param symTable Pointer to initialized SymTable
 * @param name string with the name of searched symbol
 * @returns Symbol* to the matching symbol or NULL if matching symbol was not found
*/
//Symbol* Symtable_lookup(SymTable* symTable, char* name); // renamed as it was more clear
Symbol* Symtable_get(SymTable* symTable, char* name);

/**
 * Sets value of Symbol if said Symbol exists in Symtable
 * 
 * @param symTable Pointer to initialized SymTable
 * @param name Pointer to name String (must end in \0)  
 * @param value value of Symbol
 * @returns true/false if Symbol exists
*/
// bool SymTable_set(SymTable* symTable,char* name, int value); REDUNDANT

/**
 * Gets value of Symbol if said Symbol exists in Symtable
 * 
 * @param symTable Pointer to initialized SymTable
 * @param name Pointer to name String (must end in \0)  
 * @returns value of wanted Symbol -- must think of error handling
*/
// int SymTable_get(SymTable* symTable,char* name); // REDUNDANT


#endif