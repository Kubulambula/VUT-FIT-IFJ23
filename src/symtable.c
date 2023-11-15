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

static void Symbol_free(Symbol* symbol){
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


static int countScopes(const char *str) {
    int count = 0;
    for (; *str; count += (*str++ == ':'));
        return count + 1;  // Adding 1 to account for the last substring
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
   
    //INSERT FUNCTION
    if(symbol->symbol_type == FUNCTION)
    {
        while(symTable->table[index] != NULL)
        {
            //var in main cannot have the same name as function, but vars in functions can 
            if(strcmp(symTable->table[index]->name, symbol->name) == 0)
                if(symTable->table[index]->scope == "")    
                    return ERR_SEMATIC_REDEFINED_FUNC;
            index = (index + 3) % symTable->size;        
        }

    }else
    {
        //INSERT VAR
        while(symTable->table[index] != NULL)
        {
            if(strcmp(symTable->table[index]->name, symbol->name) == 0)
                if(strcmp(symbol->scope, symTable->table[index]->scope) == 0)
                    return ERR_SEMATIC_REDEFINED_VAR;    
            index = (index + 3) % symTable->size; 
        }
    }     
    symTable->table[index] = symbol;
    symTable->count++;
    return OK;
}


//returns symbol if said symbol exists
//static Symbol* SymTable_lookup(SymTable* symTable, char* name) // renamed because it was more clear
Symbol* SymTable_get(SymTable* symTable, char* name,char* scope)
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
