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


Error generate_statements(ASTNode* statement, SymTable* symtable){
    if (statement == NULL)
        return OK;
    
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
        ERR = generate_func_call_write_args((ASTNode*)(func_call->b), &args_on_stack);
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
        if(func == NULL)
            return ERR_INTERNAL;
        generate_func_call_args((ASTNode*)(func_call->b), func->args);
    }

    printf("PUSHFRAME\nCALL %s\nPOPFRAME\n# === end call %s() ===\n", (char*)(func_call->a), (char*)(func_call->a));
    return OK;
}


Error generate_func_call_args(ASTNode* func_call_args, FuncDefArg* arg){
    if (func_call_args == NULL)
        return OK;

    ERR = generate_func_call_arg((ASTNode*)(func_call_args->a), arg);
    if (ERR)
        return ERR;
    return generate_func_call_args((ASTNode*)(func_call_args->b), arg->next);
}


Error generate_func_call_arg(ASTNode* func_call_arg, FuncDefArg* arg){
    printf("DEFVAR TF@%s\n", arg->identifier);

    // TODO assign value to the var func_call_arg->b

    return OK;
}


Error generate_func_call_write_args(ASTNode* func_call_args, unsigned* args_on_stack){
    if (func_call_args == NULL){
        *args_on_stack = 0;
        return OK;
    }
    
    ERR = generate_func_call_write_args((ASTNode*)(func_call_args->b), args_on_stack);
    if (ERR)
        return ERR;
    
    // TODO assign value to the var func_call_arg->b
    
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
    printf("RETURN\n");
    
    return OK;
}


Error generate_user_function_comment_head(ASTNode* func_head){
    ERR = generate_user_function_comment_head_signature((ASTNode*)(func_head->a));
    if (ERR)
        return ERR;

    if ((Type)(((ASTNode*)(func_head->b))->a) == TYPE_INT)
        printf("Int");
    else if ((Type)(((ASTNode*)(func_head->b))->a) == TYPE_DOUBLE)
        printf("Double");
    else if ((Type)(((ASTNode*)(func_head->b))->a) == TYPE_STRING)
        printf("String");
    else {
        printf("Bad type\n");
        return ERR_INTERNAL;
    }
    if ((bool)(((ASTNode*)(func_head->b))->a) == true)
        printf("?");

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
    printf(")->");
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



// void code_generation(ASTNode* node){
//     //ASTNode* nextNode ;

//     switch (node->type)
//     {
//     case ROOT:

//         if(node -> a != NULL){
//             code_generation(node -> a); // GENERATE USER FUNCS
//         }
//         else if(node -> b != NULL){
//             code_generation(node -> b); // NO MORE FUNCS TO GENERATE, GOING TO GLOBAL 
//         }
//         else{
//             return; // SHOULD BE DONE?
//         }

//         break;
        
//     case FUNC_DEFS:

//         if(node -> a != NULL){
//             code_generation(node -> a); // FUNC DEFINITION
//         }
//         else if(node -> b != NULL){
//             code_generation(node -> b); // ANOTHER FUNC DEF
//         }
//         else{
//             return; // NO MORE FUNCS TO GENERATE
//         }

        
//         break;

//     case STATEMENT:
            
//         break;
    
//     case FUNC_DEF:
        
//         if(node -> a != NULL){
//             code_generation(node -> a); // FUNC HEAD
//         }
//         else if(node -> b != NULL){
//             code_generation(node -> b); // STATEMENT
//         }
//         else{
//             return; 
//         }
 
//         break;

//     case FUNC_HEAD:
//         if(node -> a != NULL){
//             code_generation(node -> a); // FUNC ARGS
//         }
//         else if(node -> b != NULL){
//             code_generation(node -> b); // STATEMENT
//         }
//         else{
//             return; 
//         }
//         break;

//     case FUNC_HEAD_SIGNATURE:
//         printf("LABEL %s", (char*)(node->a));
//         printf("CREATEFRAME");
//         if(node->b != NULL)
//             generate_args(node -> b);
//         break;
        
//     default:
//         break;
//     }
    



// }
