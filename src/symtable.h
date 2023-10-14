#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>


#define SYMTABLE_INIT_SIZE 30
#define SYMTABLE_EXPAND_SIZE 20


typedef struct{
    char* name;
    int length; //name length;
    int value;
} Symbol;


typedef struct{
    Symbol** table;  //pointer to symbol table
    int size;   //table size
    int count;  //count of symbols inserted
} SymTable;


bool SymTable_init(SymTable* table, int size);

void SymTable_free(SymTable* table);

bool SymTable_insert(SymTable* table,char* name,int nameLength, int value);

//sets value of symbol
bool SymTable_set(SymTable* table,char* name, int value);

//returns value of said symbol
int SymTable_get(SymTable* table,char* name);


#endif