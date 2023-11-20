#ifndef SYNTAX_PRECEDENCE
#define SYNTAX_PRECEDENCE

#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"
#include "syntax_ll_grammar.h"

typedef struct {
	Token *array;
	int topIndex;
    int size;
} Stack;

bool Stack_Init(Stack* stack);

bool Stack_IsEmpty( const Stack * );

bool Stack_IsFull( const Stack * );

void Stack_Top( const Stack *, Token* );

void Stack_Top_Token( const Stack *stack, Token* token );

void Stack_Pop( Stack * );

void Stack_Push( Stack *, Token token );

void Stack_Dispose( Stack * );

int token2index(Token token);

bool shift_end(Stack* stack, Token stack_top_token);

int precedent_table(Token stack_top_token, Token current_token);

Error precedent(BufferString* buffer_string);

#endif