#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>


#define SYMTABLE_INIT_SIZE 30
#define SYMTABLE_EXPAND_SIZE 20


typedef struct{
    char* name;
    int value;
} Symbol;


typedef struct{
    Symbol** table;  //pointer to symbol table
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
 * Deletes all symbol in table and frees the memory 
 * 
 * @param symTable Pointer to initialized SymTable
*/
void SymTable_free(SymTable* symTable);

/**
 * Inserts new Symbol to SymTable
 * 
 * @param symTable Pointer to initialized SymTable
 * @param name Pointer to name String (must end in \0)  
 * @param value value of Symbol
 * @returns true/false if the insertion and allocation was successfull
*/
bool SymTable_insert(SymTable* symTable,char* name, int value);


/**
 * Sets value of Symbol if said Symbol exists in Symtable
 * 
 * @param symTable Pointer to initialized SymTable
 * @param name Pointer to name String (must end in \0)  
 * @param value value of Symbol
 * @returns true/false if Symbol exists
*/
bool SymTable_set(SymTable* symTable,char* name, int value);

/**
 * Gets value of Symbol if said Symbol exists in Symtable
 * 
 * @param symTable Pointer to initialized SymTable
 * @param name Pointer to name String (must end in \0)  
 * @returns value of wanted Symbol -- must think of error handling
*/
int SymTable_get(SymTable* symTable,char* name);


#endif