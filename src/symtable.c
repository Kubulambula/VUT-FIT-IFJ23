#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include <stdio.h>


Symbol* Symbol_new(){
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    if(symbol != NULL){
        symbol->symbol_type = NONE;
        symbol->name = NULL;
        symbol->type = TYPE_NIL;
        symbol->nilable = false;
        symbol->initialized = false;
        symbol->scope = 0;
        symbol->args = NULL;
        symbol->func_def = NULL;
    }
    return symbol;
}


Symbol* Symbol_copy(Symbol* to_copy){
    Symbol* symbol = Symbol_new();
    if (symbol != NULL){
        symbol->symbol_type = to_copy->symbol_type;
        symbol->name = to_copy->name;
        symbol->type = to_copy->type;
        symbol->nilable = to_copy->nilable;
        symbol->initialized = to_copy->initialized;
        symbol->scope = to_copy->scope;
        symbol->args = to_copy->args;
        symbol->func_def = to_copy->func_def;
    }

    return symbol;
}


void Symbol_free(Symbol* symbol)
{
    // stored in ast - DO NOT FREE HERE
    // free(symbol->name);
    // FuncDefArg* arg = symbol->args;
    // while (arg != NULL)
    // {
        // FuncDefArg*temp = arg->next;
        // FuncDefArg_free(arg);
        // arg = temp;
    // }
    free(symbol);
}

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

    symTable->previous = NULL;
   
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
        free(symTable->table[i]);
    
    if(symTable->previous != NULL)
        SymTable_free(symTable->previous);
    
    free(symTable->table);
    free(symTable);
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
Error SymTable_insert(SymTable* symTable, Symbol* symbol)
{
    if(symTable->count == symTable->size-1 )
    {
        if(!SymTable_resize(symTable))
            return ERR_INTERNAL;
    }

    int index = hash(symbol->name, symTable->size);
   
  

    while(symTable->table[index] != NULL)
    {
        if(strcmp(symTable->table[index]->name, symbol->name) == 0)
            return ERR_SEMATIC_REDEFINED;    
        index = (index + 3) % symTable->size; 
    }
        
    symTable->table[index] = symbol;
    symTable->count++;
    return OK;
}


//returns symbol if said symbol exists
Symbol* SymTable_get(SymTable* symTable, char* name)
{
    if (symTable->count == 0)
        return NULL;

    int index = hash(name, symTable->size);
   
    
    
    while(symTable->table[index] != NULL)
    {
        if(strcmp(symTable->table[index]->name, name) == 0)
            return symTable->table[index];
        
        index = (index + 3) % symTable->size;
    }

   
    return NULL;
}
Symbol* SymTable_get_recurse(SymTable* symTable,char*name)
{
    while(symTable != NULL)
    {
        Symbol* target=SymTable_get(symTable,name);
        if (target != NULL)
        {
            return target;
        }
        symTable=symTable->previous;
    }
    return NULL;
}