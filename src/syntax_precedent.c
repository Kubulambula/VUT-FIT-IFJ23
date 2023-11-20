#include "syntax_precedent.h"
#include <stdlib.h>

#define STACK_SIZE 50
bool ENDING_IDENTIFIER_FLAG = false;
Token TEMP_TOKEN;

bool Stack_Init(Stack* stack){
    assert(stack != NULL);
	stack->array = (Token*)malloc(sizeof(Token)*STACK_SIZE);
	stack->topIndex = 0;
    stack->size = STACK_SIZE;
    return stack->array == NULL ? false : true;
}

bool Stack_IsEmpty(const Stack *stack){
    assert(stack != NULL);
	return stack->topIndex <= 0 ? 1 : 0;	
}

bool Stack_IsFull(const Stack *stack){
    assert(stack != NULL);
	return stack->topIndex == stack->size ? 1 : 0;
}

void Stack_Top(const Stack *stack, Token* token){
    if(stack == NULL || stack->array == NULL || Stack_IsEmpty(stack)){
        *token = TOKEN_ERR_LEXICAL;
    }
	*token = stack->array[stack->topIndex - 1];
}

void Stack_Top_Token(const Stack *stack, Token* token){
    int i = 1;
    if(stack == NULL || stack->array == NULL || Stack_IsEmpty(stack)){
        *token = TOKEN_ERR_LEXICAL;
        return;
    }		
    while((stack->array[stack->topIndex - i] == PRECEDENT_E)){
        i++;
    }
	*token = stack->array[stack->topIndex - i];
}

void Stack_Pop(Stack *stack) {
	if(!(Stack_IsEmpty(stack))){
		stack->topIndex--;
	}
}

void Stack_Push(Stack *stack, Token token){
	if(Stack_IsFull(stack)){
		Token* failcheck = (Token*)realloc(stack->array, sizeof(Token)*(stack->size*2));
        if(failcheck == NULL){
            Stack_Dispose(stack);
            fprintf(stderr, "Unable to resize precedent analysis stack\n");
            return;
        }
        stack->size = stack->size * 2;
	}
		stack->array[stack->topIndex] = token;
        stack->topIndex++;
}

void Stack_Dispose(Stack *stack) {
	assert(stack != NULL);
    assert(stack->array != NULL);
	free(stack->array);
	stack->array = NULL;
    stack = NULL;
}

int token2index(Token token){
    switch(token){
        case TOKEN_IDENTIFIER:
        case TOKEN_LITERAL_DOUBLE:
        case TOKEN_LITERAL_INT:
        case TOKEN_LITERAL_STRING:
        case PRECEDENT_E:
            return 0;   // i
        case TOKEN_PARENTHESIS_LEFT:
            return 1;   // (
        case TOKEN_PARENTHESIS_RIGHT:
            return 2;   // )
        case TOKEN_EXCLAMATION:
            return 3;   // !
        case TOKEN_OPERATOR_MULTIPLICATION:
        case TOKEN_OPERATOR_DIVISION:
            return 4;   // * /
        case TOKEN_OPERATOR_PLUS:
        case TOKEN_OPERATOR_MINUS:
            return 5;   // + -
        case TOKEN_OPERATOR_EQUALS:
        case TOKEN_OPERATOR_GREATER_THAN:
        case TOKEN_OPERATOR_LESS_THAN:
        case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL:
        case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:
        case TOKEN_OPERATOR_NOT_EQUALS:
            return 6;   // == > < >= <=
        case TOKEN_NIL_COALESCING:
            return 7;   // ??
        case TOKEN_EOL:
        case TOKEN_EOF:
        case TOKEN_BRACE_LEFT:
        case TOKEN_BRACE_RIGHT:
        case PRECEDENT_END:
        case TOKEN_COMMA:
        case TOKEN_ASSIGN:
        case TOKEN_KEYWORD_FUNC:
        case TOKEN_KEYWORD_IF:
        case TOKEN_KEYWORD_WHILE:
        case TOKEN_KEYWORD_RETURN:
        case TOKEN_KEYWORD_LET:
        case TOKEN_KEYWORD_VAR:
            return 8;   // EOF EOL { } , PRECEDENT_END

        default:
            return 9;
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
            Stack_Push(stack, PRECEDENT_E);
            return true;

        case 2: // )
            if(token != TOKEN_PARENTHESIS_RIGHT)
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENT_E)
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != TOKEN_PARENTHESIS_LEFT)
                return false;
            
            Stack_Pop(stack);
            Stack_Push(stack, PRECEDENT_E);
            return true;

        case 3: // !
            if(token != TOKEN_EXCLAMATION)
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENT_E)
                return false;

            return true;

        case 4: case 5: case 6: case 7: // + - * / == <= >= < > 
            if(token != PRECEDENT_E)
                return false;

            Stack_Pop(stack);
            Stack_Top(stack, &token);
            switch(rule){
                case 4:
                    if(token2index(token) != 4)
                        return false;
                    break;

                case 5:
                    if(token2index(token) != 5)
                        return false;
                    break;

                case 6:
                    if(token2index(token) != 6)
                        return false;
                    break;

                case 7:
                    if(token2index(token) != 7)
                        return false;
            }
            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENT_E)
                return false;

            return true;

        case 8: // $
            if(token != PRECEDENT_E)
                return false;
            
            Stack_Pop(stack);
            Stack_Top(stack, &token);
            if(token != PRECEDENT_END)
                return false;

            return true;

        default:
            return false;
    }
}

int precedent_table(Token stack_top_token, Token current_precedent_token){
    int  table[10][10] = {
    { 5, 4, 2, 2, 2, 2, 2, 2, 2, 0},   // i wish there was some easier way to do this :')
    { 1, 1, 3, 1, 1, 1, 1, 1, 0, 0},   // 0 -> error
    { 0, 0, 2, 2, 2, 2, 2, 2, 2, 0},   // 1 -> <
    { 1, 1, 2, 2, 2, 2, 2, 2, 2, 0},   // 2 -> >
    { 1, 1, 2, 1, 2, 2, 2, 2, 2, 0},   // 3 -> =
    { 1, 1, 2, 1, 1, 2, 2, 2, 2, 0},   // 4 -> func()
    { 1, 1, 2, 1, 1, 1, 2, 2, 2, 0},   // 5 -> > (identifier ukoncujici expression)
    { 1, 1, 2, 1, 1, 1, 1, 2, 2, 0},
    { 1, 1, 0, 1, 1, 1, 1, 1, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    return table[token2index(stack_top_token)][token2index(current_precedent_token)];
}

/* funkce provede precedenční analýzu výrazu a sestaví podle něj btree.
* funkce nepočítá s epsilon pravidly (prázdný výraz NENÍ validní výraz)
 */
Error precedent(BufferString* buffer_string){
    Stack stack;
    Token top;
    int state;
    if(token2index(CURRENT_TOKEN) > 7) //checks for empty expression
        return ERR_SYNTAX;
    if(Stack_Init(&stack) == false)
        return ERR_INTERNAL;

    Stack_Push(&stack, PRECEDENT_END);
    Stack_Top_Token(&stack, &top);
    while(top != PRECEDENT_END || token2index(CURRENT_TOKEN) != 8){
        ll_log("precedent");
        state = precedent_table(top, CURRENT_TOKEN);
        switch(state){
            case 0:
                Stack_Dispose(&stack);
                return ERR_SYNTAX;

            case 1: case 3:
                Stack_Push(&stack, CURRENT_TOKEN);
                Stack_Top_Token(&stack, &top);
                //if(token2index(CURRENT_TOKEN) < 8)            // nevím proč jsem to sem dával ale mám strach to smazat
                     CURRENT_TOKEN = get_token(buffer_string, true);
                break;

            case 5:
                ENDING_IDENTIFIER_FLAG = true;
                TEMP_TOKEN = CURRENT_TOKEN;
                CURRENT_TOKEN = PRECEDENT_END;

            case 2:
                if(!shift_end(&stack, top)){
                    Stack_Dispose(&stack);
                    return ERR_SYNTAX;
                }
                Stack_Top_Token(&stack, &top);
                if(precedent_table(top, CURRENT_TOKEN) != 2 && precedent_table(top, CURRENT_TOKEN) != 0 && !ENDING_IDENTIFIER_FLAG){
                    Stack_Push(&stack, CURRENT_TOKEN);
                    Stack_Top_Token(&stack, &top);
                    CURRENT_TOKEN = get_token(buffer_string, true);
                }
                break;

            case 4:
                //unget_token(buffer_string);       --bude se hodit v budoucnu
                if(ll_func_call(buffer_string)){
                    Stack_Dispose(&stack);
                    return ERR_SYNTAX;
                }
                Stack_Top_Token(&stack, &top);
                if(CURRENT_TOKEN == TOKEN_EOL)
                    CURRENT_TOKEN = get_token(buffer_string, true);
        }
    }
    Stack_Dispose(&stack);
    if(ENDING_IDENTIFIER_FLAG)
        CURRENT_TOKEN = TEMP_TOKEN;
    ENDING_IDENTIFIER_FLAG = false;
    return OK;
}