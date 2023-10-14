#include <stdlib.h>

#include "symtable.h"

//TO DO
// Is this supposed to be static?
int hash(char* name,int nameLength)
{
    return 0;
}

bool SymTable_init(SymTable* table,int size)
{
    if(size <=0)
        return false;

    table = malloc(sizeof(SymTable));
    if(table == NULL)
        return false;
    
    table->size=size;
    table->table = malloc(size*sizeof(Symbol*)); 
    if(table->table == NULL)
    {
        free(table);    
        return false;
    }
    
    return true;
}

void SymTable_free(SymTable* table)
{
    for(int i=0;i<table->size;i++)
        free(table->table[i]);
    free(table);
}

// Is this supposed to be static?
SymTable* SymTable_resize(SymTable* table)
{
    int newSize = table->size + SYMTABLE_EXPAND_SIZE;
    if(newSize % 3 == 0)
        newSize++;
    SymTable* newTable;
    SymTable_init(newTable,newSize);
    for(int i = 0;i<table->size;i++)
    {
        if(!SymTable_insert(newTable,table->table[i]->name,table->table[i]->length,table->table[i]->value))
        {
            SymTable_free(newTable);
            SymTable_free(table);
            return NULL;
        }
    }
    
    return newTable;
}


//TO DO
// Is this supposed to be static?
//returns symbol if said symbol exists
Symbol* SymTable_lookup(SymTable* table,char* name,int nameLength)
{
    if (table->count==0)
        return NULL;


    int index= hash(name,nameLength) % table->size;

    bool found=true;
    
    if(table->table[index] == NULL)
        return NULL;
    
    
    do
    {
        if (nameLength == table->table[index]->length)
        {

            for (int k = 0; k<nameLength;k++)
                if(name[k] != table->table[index]->name[k])
                    found = false;
            if(found == true)
                return table->table[index];
        }
        found = true;
        index++;
    }while(table->table[index] !=NULL); 

    return NULL;
}

//TO DO
//inserts new symbol
bool SymTable_insert(SymTable* table,char* name,int nameLength, int value)
{

    return false;
}

//TO DO
//sets value of symbol
bool SymTable_set(SymTable* table,char* name, int value)
{
    return false;
}

//TO DO
//returns value of said symbol
int SymTable_get(SymTable* table,char* name)
{
    return 0;
}
