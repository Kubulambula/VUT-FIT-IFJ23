#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>
void printSym(SymTable *table)
{
    printf("\n");
    
     for (int i = 0; i < SYMTABLE_INIT_SIZE; i++)
    {
        if(table->table[i] != NULL)
        {
            printf("%d: %s %s\n",i,table->table[i]->name,table->table[i]->scope);
        }
    }
}
int main(int argc, char const *argv[])
{
    SymTable *table = malloc(sizeof(SymTable));
    SymTable_init(table);


    for (int i = 0; i < 5; i++)
    {
        Symbol *symbol = malloc(sizeof(Symbol));
        symbol->name = malloc(strlen("foo")+2);
        strcpy(symbol->name,"foo");
        symbol->name[2]=(char)i+48;
        //symbol->scope = malloc(strlen("foo")+1);
        //strcpy(symbol->scope,"foo");
        symbol->scope=NULL;
        

        printf("%d ",SymTable_insert(table,symbol));
            
    }
    
    Symbol *symbol1 = malloc(sizeof(Symbol));
    symbol1->name = malloc(strlen("foo")+2);
    strcpy(symbol1->name,"fo2");
    symbol1->scope = malloc(strlen("foo")+1);
    strcpy(symbol1->scope,"foo");
    SymTable_insert(table,symbol1);


    Symbol *symbol2 = malloc(sizeof(Symbol));
    symbol2->name = malloc(strlen("foo")+2);
    strcpy(symbol2->name,"fo2");
    symbol2->scope = malloc(strlen("foo")+1);
    strcpy(symbol2->scope,"foo:bar");
    SymTable_insert(table,symbol2);


    Symbol *symbol3 = malloc(sizeof(Symbol));
    symbol3->name = malloc(strlen("foo")+2);
    strcpy(symbol3->name,"fo2");
    symbol3->scope = malloc(strlen("foo")+1);
    strcpy(symbol3->scope,"foo:bar:func");
    SymTable_insert(table,symbol3);



    printSym(table);
    printf("\n");
    Symbol *target = SymTable_get(table,"fo2",NULL);
    if(target == NULL)
        printf("fail\n");
    else
        printf("%s\n%s\n",target->name,target->scope);

    SymTable_free(table);
    return 0;
}
