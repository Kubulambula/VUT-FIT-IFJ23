#include "syntax_precedence.h"

#define STACK_SIZE 50

void Stack_Init( Stack *stack ) {
	if(stack != NULL){
		stack->array = malloc(sizeof(char)*STACK_SIZE);
		if(stack->array == NULL){
			
		}
		else{
			stack->topIndex = -1;
            stack->size = STACK_SIZE;
		}
	}
}

bool Stack_IsEmpty( const Stack *stack ) {
	return stack->topIndex < 0 ? 1 : 0;	
}

bool Stack_IsFull( const Stack *stack ) {
	return (stack->topIndex + 1) == STACK_SIZE ? 1 : 0;
}

void Stack_Top( const Stack *stack, Token* token ) {
	*token = stack->array[stack->topIndex];
}

void Stack_Top_Token( const Stack *stack, Token* token ) {
    int i = 0;
    if(Stack_IsEmpty(stack)){
		*token = TOKEN_ERR;		
	}
    while((stack->array[stack->topIndex - i] == PRECEDENCE_E)){
        i++;
    }
	*token = stack->array[stack->topIndex];
}

void Stack_Pop( Stack *stack ) {
	if(!(Stack_IsEmpty(stack))){
		stack->topIndex--;
	}
}

void Stack_Push( Stack *stack, Token token ){
	if(Stack_IsFull(stack)){
		stack->array = realloc(stack->array, sizeof(char)*(stack->size + STACK_SIZE));
	}
		stack->topIndex++;
		stack->array[stack->topIndex] = token;
}

void Stack_Dispose( Stack *stack ) {
	if(stack != NULL){
		free(stack->array);
		stack->array = NULL;
	}
}

int token2index(Token token){
    switch(token){
        case TOKEN_IDENTIFIER:
        case TOKEN_LITERAL_DOUBLE:
        case TOKEN_LITERAL_INT:
        case TOKEN_LITERAL_STRING:
        case PRECEDENCE_E:
            return 0;
        case TOKEN_PARENTHESIS_LEFT:
            return 1;
        case TOKEN_PARENTHESIS_RIGHT:
            return 2;
        case TOKEN_EXCLAMATION:
            return 3;
        case TOKEN_OPERATOR_MULTIPLICATION:
        case TOKEN_OPERATOR_DIVISION:
            return 4;
        case TOKEN_OPERATOR_PLUS:
        case TOKEN_OPERATOR_MINUS:
            return 5;
        case TOKEN_OPERATOR_EQUALS:
        case TOKEN_OPERATOR_GREATER_THAN:
        case TOKEN_OPERATOR_LESS_THAN:
        case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL:
        case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:
            return 6;
        case TOKEN_NIL_COALESCING:
            return 7;
        case TOKEN_EOL:
        case TOKEN_EOF:
        case TOKEN_BRACE_LEFT:
        case TOKEN_BRACE_RIGHT:
        case PRECEDENCE_END:
            return 8;
    }
}

bool shift_end(Stack* stack, Token stack_top_token){
    Token token;
    int rule = token2index(stack_top_token);

    Stack_Top(stack, &token);
    switch(rule){
        case 0: // i
            if(token2index(token) != 0)
                return false;

            Stack_Pop(stack);
            Stack_Push(stack, PRECEDENCE_E);
            return true;

        case 2: // )
            if(token != TOKEN_PARENTHESIS_LEFT);
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENCE_E);
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != TOKEN_PARENTHESIS_RIGHT);
                return false;
            
            Stack_Pop(stack);
            Stack_Push(stack, PRECEDENCE_E);
            return true;

        case 3: // !
            if(token != TOKEN_EXCLAMATION)
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENCE_E)
                return false;

            //Stack_Pop(&stack);
            //Stack_Push(&stack, PRECEDENCE_E);
            return true;

        case 4: // * /
        case 5: // + -
        case 6: // == != < > <= >=
        case 7: // ??
            if(token != PRECEDENCE_E);
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            switch(rule){
                case 4:
                    if(token2index(token) != 4);
                        return false;
                    break;

                case 5:
                    if(token2index(token) != 5);
                        return false;
                    break;

                case 6:
                    if(token2index(token) != 6);
                        return false;
                    break;

                case 7:
                    if(token2index(token) != 7);
                        return false;
            }
            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENCE_E);
                return false;

            return true;

        case 8: // $
            if(token != PRECEDENCE_E);
                return false;
            
            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENCE_END);
                return false;

            return true;

        default:
            return false;
    }
}

int precedence_table(Token stack_top_token, Token current_token){
    int  table[9][9] = {
    // i wish there was another way as well
    // 0 -> error
    // 1 -> <
    // 2 -> >
    // 3 -> =
    { 0, 0, 2, 0, 2, 2, 2, 2, 2},
    { 1, 1, 0, 1, 1, 1, 1, 1, 0},
    { 0, 0, 2, 0, 2, 2, 2, 2, 2},
    { 1, 1, 2, 1, 2, 2, 2, 2, 2},
    { 1, 1, 2, 1, 1, 2, 2, 2, 2},
    { 1, 1, 2, 1, 1, 1, 2, 2, 2},
    { 1, 1, 2, 1, 1, 1, 1, 2, 2},
    { 1, 1, 2, 1, 1, 1, 1, 1, 2},
    { 1, 1, 0, 1, 1, 1, 1, 1, 0},
    };
    return table[token2index(stack_top_token)][token2index(current_token)];
}


bool precedenc(BufferString* buffer_string){
    Stack stack;
    Token top;
    Token current;
    int state;
    Stack_Init(&stack);
    if(stack.array == NULL){
        return false;
    }

    current = get_next_token(buffer_string);
    Stack_Init(&stack);
    Stack_Push(&stack, PRECEDENCE_END);
    Stack_Top_Token(&stack, &top);
    while(top != PRECEDENCE_END || token2index(current) != 8){
        state = precedence_table(top, current);
        switch(state){
            case 0:
                fprintf(stderr, "Invalid expressions\n");
                return false;
                break; 
            case 1:
                Stack_Push(&stack, current);
                break;
            case 2:
                if(shift_end(&stack, top)){
                    Stack_Push(&stack, current);
                }
                else{
                    return false;
                }
                break;
            //case 3:
        }
        Stack_Top_Token(&stack, &top);
        current = get_next_token(buffer_string);
    }
    Stack_Dispose(&stack);
    return true;
}