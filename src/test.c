#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>
void printSym(SymTable table)
{
    printf("\n");
    
     for (int i = 0; i < SYMTABLE_INIT_SIZE; i++)
    {
        if(table.table[i] != NULL)
        {
            printf("%s %s\n",table.table[i]->name,table.table[i]->scope);
        }
    }
}
int main(int argc, char const *argv[])
{
    SymTable table;
    SymTable_init(&table);


    for (int i = 0; i < 5; i++)
    {
        Symbol *symbol = malloc(sizeof(Symbol));
        symbol->name = malloc(strlen("foo")+2);
        strcpy(symbol->name,"foo");
        symbol->name[2]=(char)i+48;
        symbol->scope = malloc(strlen("foo")+1);
        strcpy(symbol->scope,"");


        printf("%d ",SymTable_insert(&table,symbol));
            
    }
    
    printSym(table);

    Symbol *target = SymTable_get(&table,"fo2","bar");
    if(target == NULL)
        printf("fail\n");
    else
        printf("%s\n%s\n",target->name,target->scope);
    return 0;
}
