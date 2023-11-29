#include <stdlib.h>
#include <assert.h>
#include "syntax_precedent.h"
#include "syntax_ll.h"

#define STACK_SIZE 10
bool ENDING_IDENTIFIER_FLAG = false;
Token TEMP_TOKEN;

bool Stack_Init(Stack* stack, enum stack_type type){
    assert(stack != NULL);
	stack->elements = malloc(STACK_SIZE*sizeof(union data));
    stack->type = type;
	stack->topIndex = -1;
    stack->size = STACK_SIZE;
    return stack->elements == NULL ? false : true;
}

bool Stack_IsEmpty(const Stack *stack){
    assert(stack != NULL);
	return stack->topIndex < 0 ? 1 : 0;	
}

bool Stack_IsFull(const Stack *stack){
    assert(stack != NULL);
	return stack->topIndex >= stack->size-1 ? 1 : 0;
}

void Stack_Top_Token(const Stack *stack, Token* token){
    if(stack != NULL && stack->elements != NULL && !Stack_IsEmpty(stack)){
        *token = stack->elements[stack->topIndex].token;
    }
    else{
        *token = TOKEN_ERR_LEXICAL;
    }
}

void Stack_Top_Node(const Stack *stack, exp_node **node){
    if(stack != NULL && stack->elements != NULL && !Stack_IsEmpty(stack)){
        *node = stack->elements[stack->topIndex].node;
    }
    else{
        node = NULL;
    }
}

void Stack_Top_Value(const Stack *stack, union literalValue *value){
    if(stack != NULL && stack->elements != NULL && !Stack_IsEmpty(stack)){
        *value = stack->elements[stack->topIndex].value;
    }
}

void Stack_Top_Token_Literal(const Stack *stack, Token* token){
    int i = 0;
    if(stack == NULL || stack->elements == NULL || Stack_IsEmpty(stack)){
        *token = TOKEN_ERR_LEXICAL;
        return;
    }		
    while((stack->elements[stack->topIndex - i].token == PRECEDENT_E)){
        i++;
    }
	*token = (Token)stack->elements[stack->topIndex - i].token;
}

void Stack_Push(Stack *stack, union data element){
    if(Stack_IsFull(stack)){
		void* failcheck = realloc(stack->elements, sizeof(union data)*(stack->size*2));
        if(failcheck == NULL){
            Stack_Dispose(stack);
            fprintf(stderr, "Unable to resize precedent analysis stack\n");
            return;
        }
        stack->size = stack->size * 2;
	}
    stack->topIndex++;
	stack->elements[stack->topIndex] = element;
}

void Stack_Pop(Stack *stack){
    if(!(Stack_IsEmpty(stack))){
		stack->topIndex--;
	}
}

void Stack_Dispose(Stack *stack){
    if(stack != NULL){
        free(stack->elements);
        stack->elements = NULL;
        stack = NULL;
    }
}

void Stack_Purge(exp_node *node){
    if(node != NULL){
        Stack_Purge(node->left);
        Stack_Purge(node->right);
        free(node);
    }
}

void Add_token(Stack *tokenStack, Stack *valueStack, Token token, BufferString *buffer_string){
    union literalValue value;
    union data data;
    data.token = token;
    Stack_Push(tokenStack, data);
    switch(token){
        case TOKEN_IDENTIFIER: case TOKEN_LITERAL_STRING:
            value.s = BufferString_get_as_string(buffer_string);
            data.value = value;
            Stack_Push(valueStack, data);
            break;
        case TOKEN_LITERAL_INT:
            value.i = BufferString_get_as_int(buffer_string);
            data.value = value;
            Stack_Push(valueStack, data);
            break;
        case TOKEN_LITERAL_DOUBLE:
            value.d = BufferString_get_as_double(buffer_string);
            data.value = value;
            Stack_Push(valueStack, data);
            break;
        default:
            break;
    }
}

exp_node *new_leaf(Token type, union literalValue value){
    exp_node* leaf = (exp_node*)malloc(sizeof(exp_node));
    if(leaf != NULL){
        leaf->left = NULL;
        leaf->right = NULL;
        leaf->type = type;
        leaf->value = value;
    }
    return leaf;
}

exp_node* new_node(exp_node* left, exp_node* right, Token type){
    exp_node* node = (exp_node*)malloc(sizeof(exp_node));
    if(node != NULL){
        node->left = left;
        node->right = right;
        node->type = type;
    }
    return node;
}

Error shift_end(Stack *tokenStack, Stack *nodeStack, Stack *valueStack, Token shift_type){
    Token token;
    Token temp_token;
    exp_node *node;
    exp_node *left;
    exp_node *right;
    union literalValue value;
    union data data;
    Stack_Top_Token(tokenStack, &token);
    switch(token2index(shift_type)){

        case 0: // i
            if(token2index(token) != 0)
                return ERR_SYNTAX;
            Stack_Top_Value(valueStack, &value);
            Stack_Pop(valueStack);
            node = new_leaf(token, value);
            if(node == NULL)
                return ERR_INTERNAL;
            data.node = node;
            Stack_Push(nodeStack, data);
            Stack_Pop(tokenStack);
            data.token = PRECEDENT_E;
            Stack_Push(tokenStack, data);
            return OK;

        case 2: // )
            if(token != TOKEN_PARENTHESIS_RIGHT)
                return ERR_SYNTAX;
            Stack_Pop(tokenStack);
            Stack_Top_Token(tokenStack, &token);
            if(token != PRECEDENT_E)
                return ERR_SYNTAX;
            Stack_Pop(tokenStack);
            Stack_Top_Token(tokenStack, &token);
            if(token != TOKEN_PARENTHESIS_LEFT)
                return ERR_SYNTAX;
            Stack_Pop(tokenStack);
            data.token = PRECEDENT_E;
            Stack_Push(tokenStack, data);
            return OK;

        case 3: // !
            if(token != TOKEN_EXCLAMATION)
                return ERR_SYNTAX;
            Stack_Pop(tokenStack);
            Stack_Top_Token(tokenStack, &token);
            if(token != PRECEDENT_E)
                return ERR_SYNTAX;
            Stack_Top_Node(nodeStack, &left);
            if(left == NULL)
                return ERR_INTERNAL;
            Stack_Pop(nodeStack);
            node = new_node(left, NULL, TOKEN_EXCLAMATION);
            if(node == NULL)
                return ERR_INTERNAL;
            data.node = node;
            Stack_Push(nodeStack, data);
            return OK;

        case 4: case 5: case 6: case 7: // + - * / == <= >= < > ??
            if(token != PRECEDENT_E)
                return ERR_SYNTAX;
            Stack_Pop(tokenStack);
            Stack_Top_Token(tokenStack, &token);
            switch(token2index(shift_type)){
                case 4:
                    if(token2index(token) != 4)
                        return ERR_SYNTAX;
                    break;
                case 5:
                    if(token2index(token) != 5)
                        return ERR_SYNTAX;
                    break;
                case 6:
                    if(token2index(token) != 6)
                        return ERR_SYNTAX;
                    break;
                case 7:
                    if(token2index(token) != 7)
                        return ERR_SYNTAX;
                    break;
                default:
                    return ERR_SYNTAX;
            }
            temp_token = token;
            Stack_Pop(tokenStack);
            Stack_Top_Token(tokenStack, &token);
            if(token != PRECEDENT_E)
                return ERR_SYNTAX;

            Stack_Top_Node(nodeStack, &right);
            Stack_Pop(nodeStack);
            Stack_Top_Node(nodeStack, &left);
            Stack_Pop(nodeStack);
            if(left == NULL || right == NULL){   // sussy memory leak
                free(left);
                free(right);
                return ERR_INTERNAL;
            }
            node = new_node(left, right, temp_token);
            if(node == NULL)
                return ERR_INTERNAL;
            data.node = node;
            Stack_Push(nodeStack, data);
            return OK;

        case 8: //$
        case 9:
        default:
            return ERR_SYNTAX;
    }
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


int precedent_table(Token stack_top_token, Token current_precedent_token){
    int  table[10][10] = {
    { 5, 4, 2, 2, 2, 2, 2, 2, 2, 0},   // i wish there was some easier way to do this :')
    { 1, 1, 3, 1, 1, 1, 1, 1, 0, 0},   // 0 -> error
    { 5, 0, 2, 2, 2, 2, 2, 2, 2, 0},   // 1 -> <
    { 5, 0, 2, 2, 2, 2, 2, 2, 2, 0},   // 2 -> >
    { 1, 1, 2, 1, 2, 2, 2, 2, 2, 0},   // 3 -> =
    { 1, 1, 2, 1, 1, 2, 2, 2, 2, 0},   // 4 -> func()
    { 1, 1, 2, 1, 1, 1, 2, 2, 2, 0},   // 5 -> > (identifier ukoncujici expression)
    { 1, 1, 2, 1, 1, 1, 1, 2, 2, 0},
    { 1, 1, 0, 1, 1, 1, 1, 1, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    return table[token2index(stack_top_token)][token2index(current_precedent_token)];
}

Error precedent(BufferString* buffer_string, exp_node **node){
    Token top;
    Stack tokenStack;
    Stack nodeStack;
    Stack valueStack;
    Error err;
    int state;
    union data data;
    if(token2index(CURRENT_TOKEN) > 7) //checks for empty expression
        return ERR_SYNTAX;
    if(!Stack_Init(&tokenStack, TOKEN) || !Stack_Init(&nodeStack, NODE) || !Stack_Init(&valueStack, VALUE)){
        Stack_Dispose(&tokenStack);
        Stack_Dispose(&nodeStack);
        Stack_Dispose(&valueStack);
    }
    data.token = PRECEDENT_E;
    Stack_Push(&tokenStack, data);
    Stack_Top_Token_Literal(&tokenStack, &top);


    while(top != PRECEDENT_END || token2index(CURRENT_TOKEN) != 8){
        //ll_log("precedent");
        state = precedent_table(top, CURRENT_TOKEN);
        switch(state){
            case 0:
                Stack_Dispose(&tokenStack);
                Stack_Dispose(&nodeStack);
                Stack_Dispose(&valueStack);
                *node = NULL;
                return ERR_SYNTAX;

            case 1: case 3:
                Add_token(&tokenStack, &valueStack, CURRENT_TOKEN, buffer_string);
                Stack_Top_Token_Literal(&tokenStack, &top);
                CURRENT_TOKEN = get_token(buffer_string, true);
                break;

            case 5:
                ENDING_IDENTIFIER_FLAG = true;
                TEMP_TOKEN = CURRENT_TOKEN;
                CURRENT_TOKEN = PRECEDENT_END;
                continue;

            case 2:
                err = shift_end(&tokenStack, &nodeStack, &valueStack, top);
                if(err){
                    Stack_Dispose(&tokenStack);
                    Stack_Dispose(&nodeStack);
                    Stack_Dispose(&valueStack);
                    *node = NULL;
                    return err;
                }
                Stack_Top_Token_Literal(&tokenStack, &top);
                if(precedent_table(top, CURRENT_TOKEN) != 2 && precedent_table(top, CURRENT_TOKEN) != 0 && !ENDING_IDENTIFIER_FLAG){
                    Add_token(&tokenStack, &valueStack, CURRENT_TOKEN, buffer_string);
                    Stack_Top_Token_Literal(&tokenStack, &top);
                    CURRENT_TOKEN = get_token(buffer_string, true);
                }
                break;

            case 4:
                unget_token(buffer_string);
                char* name = BufferString_get_as_string(buffer_string);
                exp_node *node;
                exp_node *func_node;
                err = ll_func_call(buffer_string, (ASTNode**)(&func_node), name);
                if(err){
                    Stack_Dispose(&tokenStack);
                    Stack_Dispose(&nodeStack);
                    Stack_Dispose(&valueStack);
                    return err;
                }
                node = new_node(func_node, NULL, TOKEN_KEYWORD_FUNC);
                if(node == NULL){
                    Stack_Dispose(&tokenStack);
                    Stack_Dispose(&nodeStack);
                    Stack_Dispose(&valueStack);
                    return ERR_INTERNAL;
                }
                data.node = node;
                Stack_Push(&nodeStack, data);
                Stack_Pop(&tokenStack);
                data.token = PRECEDENT_E;
                Stack_Push(&tokenStack, data);
                Stack_Top_Token_Literal(&tokenStack, &top);
                CURRENT_TOKEN = get_token(buffer_string, true);
        }
    }
    Stack_Top_Node(&nodeStack, node);
    Stack_Dispose(&tokenStack);
    Stack_Dispose(&valueStack);
    free(nodeStack.elements);
    if(ENDING_IDENTIFIER_FLAG)
        CURRENT_TOKEN = TEMP_TOKEN;
    ENDING_IDENTIFIER_FLAG = false;
    return OK;
}

Error let_nil(exp_node **node, char* identifier_name){
    union literalValue name;        // Uvidíme jestli tohle pomůže
    name.s = identifier_name;       
    exp_node *left = new_leaf(TOKEN_IDENTIFIER, name);
    if (left == NULL)
        return ERR_INTERNAL;
    union literalValue nil_value;
    nil_value.i = 0;
    exp_node *right = new_leaf(TOKEN_LITERAL_NIL, nil_value);
    if (right == NULL){
        free(left);
        return ERR_INTERNAL;
    }

    *node = new_node(left, right, TOKEN_OPERATOR_NOT_EQUALS);
    if(*node == NULL){
        free(left);
        free(right);
        return ERR_INTERNAL;
    }

    return OK;
}

Error variable_expression(exp_node **node, char* identifier_name){
    union literalValue name;
    name.s = identifier_name;
    *node = new_leaf(TOKEN_IDENTIFIER, name);
    if(*node == NULL)
        return ERR_INTERNAL;
    
    return OK;
}