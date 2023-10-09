#ifndef _SYMTABLE_
#define _SYMTABLE_

#include <stdlib.h>
#include <stdbool.h>


typedef struct
{
    char* name;
    int value;
}Symbol;

typedef struct  
{

    Symbol* table[83];
    int size;


}SymTable;

void SymTable_init(SymTable* table)
{

}

void SymTable_free(SymTable* table)
{

}

SymTable* SymTable_resize(SymTable* table)
{
    return NULL;
}

//returns symbol if said symbol exists
Symbol* SymTable_lookup(SymTable* table,char* name)
{
    return NULL;
}

//inserts new symbol
bool SymTable_insert(SymTable* table,char* name, int value)
{
    return false;
}

//sets value of symbol
bool SymTable_set(SymTable* table,char* name, int value)
{
    return false;
}

//returns value of said symbol
int SymTable_get(SymTable* table,char* name)
{
    return 0;
}



#endif