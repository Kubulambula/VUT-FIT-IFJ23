#include <stdio.h>
#include "error.h"

// Prvek jednoducheho seznamu
typedef struct TonekNode {
    // Typ tokenu. (Budem resit cislem teda?)
    int type;
    // Ukazatel na následující prvek seznamu.
    struct TonekNode* next;
} TonekNode;

// Jednoduchy seznam
typedef struct TokenList{
    // Ukazatel na "hlavicku" seznamu
    TonekNode* head;
} TokenList;

// Funkce najde a vytvori token
void get_next_token();

// Svaze tokeny na stejnem radku a vrati je
void get_next_line();