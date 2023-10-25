#ifndef SYNTAX_PRECEDENCE
#define SYNTAX_PRECEDENCE

#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"

typedef struct {
	Token *array;
	int topIndex;
    int size;
} Stack;

void Stack_Init( Stack * );

bool Stack_IsEmpty( const Stack * );

bool Stack_IsFull( const Stack * );

void Stack_Top( const Stack *, Token* );

void Stack_Pop( Stack * );

void Stack_Push( Stack *, Token token );

void Stack_Dispose( Stack * );

#endif