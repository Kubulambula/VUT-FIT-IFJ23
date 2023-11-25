#ifndef SYNTAX_PRECEDENCE
#define SYNTAX_PRECEDENCE

#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"
#include "syntax_ll_grammar.h"

// Union literálů
union literalValue{
    int i;
    double d;
    char* s;
    Token t;
};

// Struktura uzlu
typedef struct{
    Token type;
    union literalValue value;
    exp_node* left;
    exp_node* right;
} exp_node;

// Struktura zásobníku používaná při precedenční anylýze
typedef struct {
	exp_node *nodeStack;            // Ukazatel na pole uzlů
    union literalValue *valueStack;  // Ukazatel na pole hodnot
	int topIndex;               // Počet uzlů v zásobníku
    int size;                   // Velikost alokovaného prostoru
} Stack;

// Inicializuje zásobník, alokuje prostor pro zásobník, nastaví topIndex na 0 
bool Stack_Init(Stack* stack);

// Vrátí true pokud je zásobník prázdný, jinka vrátí false
bool Stack_IsEmpty( const Stack * );

// Vrátí true pokud je zásobník plný a už na něj nelze přidávat další tokeny, jinak vrátí false
bool Stack_IsFull( const Stack * );

void Stack_Top_node();

void Stack_Top_value();


// Vrátí token na vrcholu zásobníku
void Stack_Top( const Stack *, Token* );

// Vrátí token na vrcholu zásobníku který není PRECEDENT_E
void Stack_Top_Token( const Stack *stack, Token* token );

// Odstraní token na vrcholu zásobníku zásobníku
void Stack_Pop( Stack * );

// Přidá token na vrchol zásobníku. Pokud na zásobníku není místo, zdvojnásobí velikost zásobníku
void Stack_Push( Stack *, Token token );

// Uvolní zásobník z paměti
void Stack_Dispose( Stack * );

// Převede token na index určený precedenční tabulkou
int token2index(Token token);

// Provádí ukončovací shift operace na zásobníku
bool shift_end(Stack* stack, Token stack_top_token);

// Implementace precedenční tabulky, vrací číslo operace kterou má precedenční analýza provést
int precedent_table(Token stack_top_token, Token current_token);

// Funkce provede precedenční analýzu výrazu a sestavý pro něj AST. Funkce nepočítá s epsilon pravidly (prázdný výraz není validní výraz)
Error precedent(BufferString* buffer_string);

#endif