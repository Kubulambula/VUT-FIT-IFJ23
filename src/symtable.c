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

static bool SymTable_init_Spec(SymTable* symTable,int size)
{
    if(size <=0)
        return false;

    symTable->size=size;
    symTable->count=0;
    
    symTable->table = calloc(size,sizeof(Symbol*)); 
    if(symTable->table == NULL)
        return false;
    
    
    return true;
}
bool SymTable_init(SymTable* symTable)
{
    return SymTable_init_Spec(symTable,SYMTABLE_INIT_SIZE);
}

void SymTable_free(SymTable* symTable)
{
    if(symTable != NULL)
    {
        for(int i=0;i<symTable->size;i++)
        {
            if(symTable->table[i]!=NULL)
            {
                free(symTable->table[i]->name);
                free(symTable->table[i]);

            }
        }
        free(symTable->table);
        free(symTable);
    }
}

static bool SymTable_resize(SymTable* symTable)
{
    int newSize = symTable->size + SYMTABLE_EXPAND_SIZE;
    if(newSize % 3 == 0)
        newSize++;
    
    Symbol** newTable= calloc(newSize,sizeof(Symbol*)); 
    SymTable temp;
    temp.table = newTable;
    temp.size = newSize;
    temp.count = 0;
    
    for(int i = 0;i<symTable->size;i++)
    {
        if(symTable->table[i] != NULL  && !SymTable_insert(&temp,symTable->table[i]->name,symTable->table[i]->value))
        {
            return false;
        }
    }

    symTable->table = newTable;
    symTable->size = newSize;
    return true;
}


//inserts new symbol
bool SymTable_insert(SymTable* symTable,char* name, int value)
{
    if(symTable->count == symTable->size-1 )
    {
        if(!SymTable_resize(symTable))
            return false;
    }

    int index = hash(name,symTable->size);
   
    while(symTable->table[index] != NULL )
    {
        if(strcmp(symTable->table[index]->name,name) == 0)
        {
            
            symTable->table[index]->value = value;
            return true;
        }
        index = (index+3 ) % symTable->size;
    }
     
    symTable->table[index] = malloc(sizeof(Symbol));
    if (symTable->table[index] == NULL)
        return false;
    
    symTable->table[index]->name = name;
    symTable->table[index]->value = value;
    symTable->count++;
    return true;
}


//returns symbol if said symbol exists
static Symbol* SymTable_lookup(SymTable* symTable,char* name)
{
    if (symTable->count==0)
        return NULL;



    int index= hash(name,symTable->size) ;
    
    
   
    while(symTable->table[index] != NULL)
    {
        
        if(strcmp(name,symTable->table[index]->name) != 0){
            index += 3;
            continue;
        }
        return symTable->table[index];
    
    }

    return NULL;
}


//sets value of symbol
bool SymTable_set(SymTable* symTable,char* name, int value)
{
    Symbol* symbol = SymTable_lookup(symTable,name);
    if (symbol == NULL)
        return false;

    symbol->value = value;
    
    return true;
}

//returns value of said symbol
int SymTable_get(SymTable* symTable,char* name)
{
    Symbol* symbol = SymTable_lookup(symTable,name);
    if (symbol == NULL)
        return 666;
    return symbol->value;
}
