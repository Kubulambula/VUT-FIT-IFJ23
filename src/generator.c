#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "generator.h"
#include "generator_pre_defined.h"
#include "buffer_string.h"  


char* format_string_to_IFJcode23_string(char* IFJ23_string){
    BufferString IFJcode23_buffer_string;
    if (!BufferString_init(&IFJcode23_buffer_string))
        return NULL;
    
    int Idx = 0;
    while(IFJ23_string[Idx] != '\0'){
        if(IFJ23_string[Idx] <= ' ' || IFJ23_string[Idx] == '#' || IFJ23_string[Idx] == '\\'){
            char asciiValue[6]; // Assuming ASCII values are 3 digits or less
            sprintf(asciiValue, "\\%03d", IFJ23_string[Idx]);
            BufferString_append_str(&IFJcode23_buffer_string, asciiValue);
        } else{
            BufferString_append_char(&IFJcode23_buffer_string, IFJ23_string[Idx]);
        }
        Idx++;
    }
    char *IFJcode23_string = BufferString_get_as_string(&IFJcode23_buffer_string);
    BufferString_free(&IFJcode23_buffer_string);
    return IFJcode23_string; // Has to be freed by caller
}


char* get_generated_literal_int(int value){
    unsigned length = snprintf(NULL, 0, "int@%d", value);
    char* str = (char*)malloc(length + 1);
    if (str == NULL)
        return NULL;

    sprintf(str, "int@%d", value);
    return str;
}


char* get_generated_literal_double(double value){
    unsigned length = snprintf(NULL, 0, "float@%a", value);
    char* str = (char*)malloc(length + 1);
    if (str == NULL)
        return NULL;

    sprintf(str, "float@%a", value);
    return str;
}


char* get_generated_literal_string(char* value){
    char* IFJcode23_str = format_string_to_IFJcode23_string(value);
    if (IFJcode23_str == NULL)
        return NULL;

    unsigned length = snprintf(NULL, 0, "string@%s", IFJcode23_str);
    char* str = (char*)malloc(length + 1);
    if (str == NULL){
        free(IFJcode23_str);
        return NULL;
    }

    sprintf(str, "string@%s", IFJcode23_str);
    free(IFJcode23_str);
    return str;
}


char* get_generated_literal_nil(){
    char* str = (char*)malloc(sizeof(char) * 8);
    if (str == NULL)
        return NULL;
    
    strcpy(str, "nil@nil\0");
    return str;
}


Error generate_expression(exp_node* expression, SymTable* symtable){
    char frame_name[3]; // helper variable for getting variable frame
    char* literal_value; // helper variable for outputing literal values

    switch (expression->type){
        case TOKEN_OPERATOR_PLUS:
            ERR = generate_expression(expression->left, symtable);
            if (ERR)
                return ERR;
            ERR = generate_expression(expression->right, symtable);
            if (ERR)
                return ERR;
            
            printf("ADDS\n");
            return OK;
        
        case TOKEN_CONCATENATE: // fancy + for strings
            fprintf(stderr, "Not implemented yet\n");
            return ERR_INTERNAL;
            break;
        
        case TOKEN_OPERATOR_MINUS:
            ERR = generate_expression(expression->left, symtable);
            if (ERR)
                return ERR;
            ERR = generate_expression(expression->right, symtable);
            if (ERR)
                return ERR;
            
            printf("SUBS\n");
            return OK;
        
        case TOKEN_OPERATOR_MULTIPLICATION:
            ERR = generate_expression(expression->left, symtable);
            if (ERR)
                return ERR;
            ERR = generate_expression(expression->right, symtable);
            if (ERR)
                return ERR;
            
            printf("MULS\n");
            return OK;
        
        case TOKEN_OPERATOR_DIVISION: // we have to implement integer division
            ERR = generate_expression(expression->left, symtable);
            if (ERR)
                return ERR;
            ERR = generate_expression(expression->right, symtable);
            if (ERR)
                return ERR;
            
            printf("DIVS\n");
            return OK;
        
        case TOKEN_EXCLAMATION:
            fprintf(stderr, "Not implemented yet\n");
            return ERR_INTERNAL;
            break;
        
        case TOKEN_NIL_COALESCING:
            fprintf(stderr, "Not implemented yet\n");
            return ERR_INTERNAL;
            break;
        
        case TOKEN_KEYWORD_FUNC:
            return generate_func_call((ASTNode*)(expression->left), symtable);

        case TOKEN_IDENTIFIER:
            // somehow detect the Frame
            if (SymTable_get(symtable, (expression->value).s) == NULL)
                strcpy(frame_name, "LF");
            else
                strcpy(frame_name, "GF");

            printf("PUSHS   %s@%s\n", frame_name, (expression->value).s);
            return OK;
        
        case TOKEN_LITERAL_INT:
            literal_value = get_generated_literal_int((expression->value).i);
            if (literal_value == NULL)
                return ERR_INTERNAL;
            
            printf("PUSHS   %s\n", literal_value);
            free(literal_value);
            return OK;
        
        case TOKEN_LITERAL_DOUBLE:
            literal_value = get_generated_literal_double((expression->value).d);
            if (literal_value == NULL)
                return ERR_INTERNAL;
            
            printf("PUSHS   %s\n", literal_value);
            free(literal_value);
            return OK;
        
        case TOKEN_LITERAL_STRING:
            literal_value = get_generated_literal_string((expression->value).s);
            if (literal_value == NULL)
                return ERR_INTERNAL;
            
            printf("PUSHS   %s\n", literal_value);
            free(literal_value);
            return OK;
        
        case TOKEN_LITERAL_NIL:
            literal_value = get_generated_literal_nil();
            if (literal_value == NULL)
                return ERR_INTERNAL;
            
            printf("PUSHS   %s\n", literal_value);
            free(literal_value);
            return OK;
        
        default:
            fprintf(stderr, "Not implemented yet\n");
            return ERR_INTERNAL;
            break;
    }
}


Error generate_statements(ASTNode* statement, SymTable* symtable){
    if (statement == NULL)
        return OK;
    
    printf("# statement here\n");
    switch (((ASTNode*)(statement->a))->type){
        case VAR_DEF:
            break;

        case LET_DEF:
            break;

        case ASSIGN:
            break;

        case FUNC_CALL:
            ERR = generate_func_call((ASTNode*)(statement->a), symtable);
            if (ERR)
                return ERR;
            break;

        case IFELSE:
            break;

        case WHILE:
            break;

        case RETURN:
            break;
        
        default:
            fprintf(stderr, "Invalid statement!\n");
            return ERR_INTERNAL;
    }

    return generate_statements((ASTNode*)(statement->b), symtable);
}


Error generate_func_call(ASTNode* func_call, SymTable* symtable){
    printf("\n# === call %s() ===\nCREATEFRAME\n", (char*)(func_call->a));


    // write() is a special little function with args on the stack
    if (strcmp((char*)(func_call->a), "write") == 0){
        unsigned args_on_stack = 0;
        ERR = generate_func_call_write_args((ASTNode*)(func_call->b), &args_on_stack, symtable);
        if (ERR)
            return ERR;
        
        printf("DEFVAR      TF@argCnt\n");

        char* argCnt_value = get_generated_literal_int(args_on_stack);
        if (argCnt_value == NULL)
            return ERR_INTERNAL;

        printf("MOVE        TF@argCnt %s\n", argCnt_value);
        free(argCnt_value);

    } else {
        Symbol* func = SymTable_get(symtable, (char*)(func_call->a));
        fprintf(stderr, "Function does not exist\n");
        if(func == NULL)
            return ERR_INTERNAL;
        generate_func_call_args((ASTNode*)(func_call->b), func->args, symtable);
    }

    printf("PUSHFRAME\nCALL %s\nPOPFRAME\n# === end call %s() ===\n", (char*)(func_call->a), (char*)(func_call->a));
    return OK;
}


Error generate_func_call_args(ASTNode* func_call_args, FuncDefArg* arg, SymTable* symtable){
    if (func_call_args == NULL)
        return OK;

    ERR = generate_func_call_arg((ASTNode*)(func_call_args->a), arg, symtable);
    if (ERR)
        return ERR;
    return generate_func_call_args((ASTNode*)(func_call_args->b), arg->next, symtable);
}


Error generate_func_call_arg(ASTNode* func_call_arg, FuncDefArg* arg, SymTable* symtable){
    ERR = generate_expression((exp_node*)(func_call_arg->b), symtable);
    if (ERR)
        return ERR;
    
    printf("DEFVAR  TF@%s\n", arg->identifier);
    printf("POPS    TF@%s\n", arg->identifier);
    
    return OK;
}


Error generate_func_call_write_args(ASTNode* func_call_args, unsigned* args_on_stack, SymTable* symtable){
    if (func_call_args == NULL){
        *args_on_stack = 0;
        return OK;
    }
    
    ERR = generate_func_call_write_args((ASTNode*)(func_call_args->b), args_on_stack, symtable);
    if (ERR)
        return ERR;
    
    ASTNode* arg = (ASTNode*)(func_call_args->a);
    ERR = generate_expression((exp_node*)(arg->b), symtable);
    if (ERR)
        return ERR;
    
    *args_on_stack = *args_on_stack + 1;
    return OK;
}


Error generate_assing(ASTNode* assign, SymTable* symtable){
    bool is_global = symtable->previous == NULL;
    
    printf((is_global ? "POPS        GF@" : "POPS        LF@"));
    return OK;
}


Error generate_user_functions(ASTNode* func_defs, SymTable* symtable){
    if (func_defs == NULL)
        return OK;
    
    ERR = generate_user_function((ASTNode*)(func_defs->b), symtable);
    if (ERR)
        return ERR;

    return generate_user_functions((ASTNode*)(func_defs->a), symtable);
}


Error generate_user_function(ASTNode* func_def, SymTable* symtable){
    ERR = generate_user_function_comment_head((ASTNode*)(func_def->a));
    if (ERR)
        return ERR;

    printf("LABEL           %s\n", (char*)(((ASTNode*)(((ASTNode*)(func_def->a))->a))->a));
    printf("# Function code goes here\n");
    
    ERR = generate_statements((ASTNode*)(func_def->b), symtable);
    if (ERR)
        return ERR;
        
    printf("RETURN\n");
    
    return OK;
}


Error generate_user_function_comment_head(ASTNode* func_head){
    ERR = generate_user_function_comment_head_signature((ASTNode*)(func_head->a));
    if (ERR)
        return ERR;
    if ((Type)(((ASTNode*)(func_head->b))->a) != TYPE_NIL){
        if ((Type)(((ASTNode*)(func_head->b))->a) == TYPE_INT)
            printf("->Int");
        else if ((Type)(((ASTNode*)(func_head->b))->a) == TYPE_DOUBLE)
            printf("->Double");
        else if ((Type)(((ASTNode*)(func_head->b))->a) == TYPE_STRING)
            printf("->String");
        
        if ((bool)(((ASTNode*)(func_head->b))->b) == true)
            printf("?");
    }

    printf(" (user-defined)\n");
    return OK;
}


Error generate_user_function_comment_head_signature(ASTNode* func_head_signature){
    printf("\n\n# Function %s(", (char*)(func_head_signature->a));

    FuncDefArg* arg = (FuncDefArg*)(func_head_signature->b);
    while (arg != NULL){
        printf("%s %s: ", (arg->name ? arg->name : "_"), arg->identifier);
        if (arg->type == TYPE_INT)
            printf("Int");
        else if (arg->type == TYPE_DOUBLE)
            printf("Double");
        else if (arg->type == TYPE_STRING)
            printf("String");
        else {
            printf("Bad type\n");
            return ERR_INTERNAL;
        }
        if (arg->nilable)
            printf("?");

        arg = arg->next;
        if (arg != NULL)
            printf(", ");
    }
    printf(")");
    return OK;
}


Error generate_main_body(ASTNode* statements, SymTable* symtable){
    printf("%s", MAIN); 
    return generate_statements(statements, symtable);
}


Error generate_code(ASTNode* root, SymTable* symtable){
    printf("%s", HEADER);

    printf("%s", FUNCTIONS_BUILT_IN);

    ERR = generate_user_functions((ASTNode*)(root->a), symtable);
    if (ERR)
        return ERR;

    ERR = generate_main_body((ASTNode*)(root->b), symtable);
    if (ERR)
        return ERR;
    
    printf("%s", FOOTER);

    return OK;
}
