#ifndef SYNTAX_PRECEDENCE
#define SYNTAX_PRECEDENCE

#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"
// #include "ast.h"
// #include "syntax_ll.h"

// Union literálů
// union literalValue{
//     int i;      
//     double d;
//     char* s;
// };

// Union literalu
union literalValue{
    int i;
    double d;
    char* s;
};


// Struktura uzlu výrazu
typedef struct exp_node_t{
    Token type;                 // typ uzlu
    union literalValue value;   // hodnota literálu / název proměnné
    struct exp_node_t *left;    // levý potomek
    struct exp_node_t *right;   // pravý potomek
} exp_node;

enum stack_type{
    TOKEN,
    NODE,
    VALUE
};
    
// Union datových typů, zásobník pracuje s jedním z těchto typů 
union data {
    Token token;
    exp_node *node;
    union literalValue value;
};

// Struktura zásobníku
typedef struct {
    enum stack_type type;   // Typ zásobníku
    union data *elements;   // Ukazatel na pole prvků
	int topIndex;           // index nejvrchnějšího prvku
    int size;               // Velikost zásobníku
} Stack;

// Funkce inicializuje zásobník
bool Stack_Init(Stack* stack, enum stack_type type);

// Vrátí true pokud je zásobník prázdný, jinak vrátí false
bool Stack_IsEmpty(const Stack *stack);

// Vrátí true pokud je zásobník plný, jinak vrátí false
bool Stack_IsFull(const Stack *stack);

// Prvek na vrcholu zásobníku vrátí jako token
void Stack_Top_Token(const Stack *stack, Token *token);

// Prvek na vrcholu zásobníku vrátí jako uzel
void Stack_Top_Node(const Stack *stack, exp_node **node);

// Prvek na vrcholu zásobníku vrátí jako union hodnot
void Stack_Top_Value(const Stack *stack, union literalValue *value);

// Vrátí první prvek na zásobníku který není PRECEDENT_E
void Stack_Top_Token_Literal(const Stack *stack, Token* token);

// Vloží prvek na vrchol zásobníku
void Stack_Push(Stack *stack, union data element);

// Odstraní vrchní prvek ze zásobníku
void Stack_Pop(Stack *stack);

// Uvolní zásobník
void Stack_Dispose(Stack *stack);

// Nic nezůstane
void exp_node_purge(exp_node *node);

// Alokuje nový list výrazového stromu
exp_node *new_leaf(Token type, union literalValue value);

// Alokuje nový uzel výrazového stromu
exp_node* new_node(exp_node* left, exp_node* right, Token type);

// Převede token na index určený precedenční tabulkou
int token2index(Token token);

// Provádí ukončovací shift operace na zásobníku
Error shift_end(Stack *tokenStack, Stack *nodeStack, Stack *valueStack, Token shift_type);

// Implementace precedenční tabulky, vrací číslo operace kterou má precedenční analýza provést
int precedent_table(Token stack_top_token, Token current_token);

// Funkce provede precedenční analýzu výrazu a sestavý pro něj AST. Funkce nepočítá s epsilon pravidly (prázdný výraz není validní výraz)
Error precedent(BufferString* buffer_string, exp_node **node, bool allow_empty);

Error let_nil(exp_node **node, char* identifier);

Error variable_expression(exp_node **node, char* identifier_name);


// tohle všechno může být v type:
//TOKEN_OPERATOR_PLUS,       left je levý operátor, right je pravý operátor(tohle platí pro všechny binární operátory)
//TOKEN_OPERATOR_MINUS,
//TOKEN_OPERATOR_MULTIPLICATION,
//TOKEN_OPERATOR_DIVISION,
//TOKEN_OPERATOR_LESS_THAN,
//TOKEN_OPERATOR_GREATER_THAN,
//TOKEN_OPERATOR_LESS_THAN_OR_EQUAL,
//TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL,
//TOKEN_OPERATOR_EQUALS,
//TOKEN_OPERATOR_NOT_EQUALS,
//TOKEN_EXCLAMATION, // !           left je levý operand, right je NULL
//TOKEN_NIL_COALESCING,

//TOKEN_LITERAL_INT,    v value je hodnota, left a right je NULL
//TOKEN_LITERAL_DOUBLE,
//TOKEN_LITERAL_STRING,

//TOKEN_IDENTIFIER      v value je jméno proměnné (union literalValue.s), left a right je NULL

//TOKEN_KEYWORD_FUNC    v left je přetypovaný odkaz na ASTNode, obsahující podstrom volání funkce, v right je NULL
#endif