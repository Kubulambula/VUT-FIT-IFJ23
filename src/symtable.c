#include <stdlib.h>
#include <string.h>
#include "symtable.h"


static unsigned hash(char* name,int size)
{
    unsigned long long hash = 1;
    unsigned int index = 0;
    while(name[index] != '\0')
    {
        hash *=(int)name[index];
        index++;
    }
    return hash % size;
}

static bool SymTable_init_Spec(SymTable* symTable, int size)
{
    if(size <= 0)
        return false;
    
    if(size % 3 == 0)
        size++;

    symTable->size=size;
    symTable->count=0;
    
    symTable->table = calloc(size, sizeof(Symbol*)); 
    /*if(symTable->table == NULL)
        return false;
    return true;*/
    return symTable->table != NULL;
}

bool SymTable_init(SymTable* symTable)
{
    return SymTable_init_Spec(symTable, SYMTABLE_INIT_SIZE);
}

void SymTable_free(SymTable* symTable)
{
    if(symTable == NULL)
        return;
 
    for(int i=0; i<symTable->size; i++)
        Symbol_free(symTable->table[i]);
    
    free(symTable->table);
    free(symTable);
}

void Symbol_free(Symbol* symbol){
    if(symbol == NULL)
        return;
    
    free(symbol->name);
    free(symbol);
}

static bool SymTable_resize(SymTable* symTable)
{
    int newSize = symTable->size + SYMTABLE_EXPAND_SIZE;
    
    SymTable temp;
    SymTable_init_Spec(&temp, newSize);
    
    for(int i = 0; i < symTable->size; i++)
    {
        if(symTable->table[i] != NULL && !SymTable_insert(&temp, symTable->table[i])){
            SymTable_free(&temp);
            return false;
        }
    }

    // Modify the input symtable, be cause we don't want to messup any pointers to it
    free(symTable->table);
    symTable->table = temp.table;
    symTable->size = temp.size;
    symTable->count = temp.count;
    // DO NOT FREE TEMP. The pointer to the table is now in the ownership of symTable that was passed as parameter.

    return true;
}


//inserts new symbol
bool SymTable_insert(SymTable* symTable, Symbol* symbol)
{
    if(symTable->count == symTable->size-1 )
    {
        if(!SymTable_resize(symTable))
            return false;
    }

    int index = hash(symbol->name, symTable->size);
   
    while(symTable->table[index] != NULL)
    {
        if(strcmp(symTable->table[index]->name, symbol->name) == 0)
        {   
            free(symTable->table[index]);
            symTable->table[index] = symbol;
            return true;
        }
        index = (index + 3) % symTable->size;
    }
     
    symTable->table[index] = symbol;
    symTable->count++;
    return true;
}


//returns symbol if said symbol exists
//static Symbol* SymTable_lookup(SymTable* symTable, char* name) // renamed because it was more clear
static Symbol* SymTable_get(SymTable* symTable, char* name)
{
    if (symTable->count == 0)
        return NULL;

    int index = hash(name, symTable->size);
   
    while(symTable->table[index] != NULL)
    {
        if(strcmp(name, symTable->table[index]->name) != 0){
            index += 3;
            continue;
        }
        return symTable->table[index];
    }

    return NULL;
}


//sets value of symbol
// redundant by SymTable_insert
/*bool SymTable_set(SymTable* symTable, char* name, int value)
{
    Symbol* symbol = SymTable_lookup(symTable,name);
    if (symbol == NULL)
        return false;

    symbol->value = value;
    
    return true;
}*/

//returns value of said symbol
// redundant by Symtable_lookup
/*int SymTable_get(SymTable* symTable, char* name)
{
    Symbol* symbol = SymTable_lookup(symTable,name);
    if (symbol == NULL)
        return 666;
    return symbol->value;
}*/
