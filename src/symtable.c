#include <stdlib.h>
#include <string.h>

#include "symtable.h"


static int64_t hash(char* name)
{
    int64_t hash = 0;
    int index = 0;
    while(name[index] != '\0')
    {
        hash *=(int)name[index];
    }
    return hash;
}

static bool SymTable_init_Spec(SymTable* symTable,int size)
{
    if(size <=0)
        return false;

    symTable->size=size;
    symTable->count=0;
    
    symTable->table = malloc(size*sizeof(Symbol*)); 
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
    for(int i=0;i<symTable->size;i++)
        free(symTable->table[i]);
    free(symTable->table);
    free(symTable);
}

static SymTable* SymTable_resize(SymTable* symTable)
{
    int newSize = symTable->size + SYMTABLE_EXPAND_SIZE;
    if(newSize % 3 == 0)
        newSize++;
    SymTable* newTable;
    SymTable_init_Spec(newTable,newSize);
    for(int i = 0;i<symTable->size;i++)
    {
        if(!SymTable_insert(newTable,symTable->table[i]->name,symTable->table[i]->value))
        {
            SymTable_free(newTable);
            SymTable_free(symTable);
            return NULL;
        }
    }
    
    return newTable;
}


//returns symbol if said symbol exists
static Symbol* SymTable_lookup(SymTable* symTable,char* name)
{
    if (symTable->count==0)
        return NULL;



    int index= hash(name) % symTable->size;
    
    
    bool found=true;
    while(symTable->table[index] != NULL)
    {
        
        if(strcmp(name,symTable->table[index]->name) != 0)
            found=false;
        if(found == true)
            return symTable->table[index];
    
        found = true;
        index +=3;
    }

    return NULL;
}

//inserts new symbol
bool SymTable_insert(SymTable* symTable,char* name, int value)
{

    if(symTable->size = symTable->count -1)
        SymTable_resize(symTable);
    

    int index = hash(name) % symTable->size;
    
    while(symTable->table[index] != NULL)
        index +=3;
    
    symTable->table[index] = malloc(sizeof(Symbol));
    if (symTable->table[index] == NULL)
        return false;
    
    symTable->table[index]->name = name;
    symTable->table[index]->value = value;
    return true;
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
        return 0;
    return symbol->value;
}
