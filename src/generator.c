#include <stdlib.h>
#include "generator.h"
#include "generator_pre_defined.h"
#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "buffer_string.h"  


// built-in funcs detection
// func processing
//need to create stack for frames ?


void generate_args(FuncDefArg* Arg){
    int ArgNum = 1;
    while(Arg->next != NULL){
        printf("DEFVAR TF@&%d",ArgNum);
        printf("MOVE TF@&%d GF@arg%d",ArgNum, ArgNum);  // this should be done when calling the func
        ArgNum++;
    }
}


// Looks good to me
char* process_string_literal(char* IFJ23_string){
    BufferString IFJcode23_buffer_string;
    BufferString_init(&IFJcode23_buffer_string);
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


void generate_code(){
    printf("%s", HEADER);
    printf("%s", FUNCTIONS_BUILT_IN);
    // user functions
    printf("%s", MAIN);
    // main statements
    printf("%s", FOOTER);
}



void code_generation(ASTNode* node){
    //ASTNode* nextNode ;

    switch (node->type)
    {
    case ROOT:

        if(node -> a != NULL){
            code_generation(node -> a); // GENERATE USER FUNCS
        }
        else if(node -> b != NULL){
            code_generation(node -> b); // NO MORE FUNCS TO GENERATE, GOING TO GLOBAL 
        }
        else{
            return; // SHOULD BE DONE?
        }

        break;
        
    case FUNC_DEFS:

        if(node -> a != NULL){
            code_generation(node -> a); // FUNC DEFINITION
        }
        else if(node -> b != NULL){
            code_generation(node -> b); // ANOTHER FUNC DEF
        }
        else{
            return; // NO MORE FUNCS TO GENERATE
        }

        
        break;

    case STATEMENT:
            
        break;
    
    case FUNC_DEF:
        
        if(node -> a != NULL){
            code_generation(node -> a); // FUNC HEAD
        }
        else if(node -> b != NULL){
            code_generation(node -> b); // STATEMENT
        }
        else{
            return; 
        }
 
        break;

    case FUNC_HEAD:
        if(node -> a != NULL){
            code_generation(node -> a); // FUNC ARGS
        }
        else if(node -> b != NULL){
            code_generation(node -> b); // STATEMENT
        }
        else{
            return; 
        }
        break;

    case FUNC_HEAD_SIGNATURE:
        printf("LABEL %s", (char*)(node->a));
        printf("CREATEFRAME");
        if(node->b != NULL)
            generate_args(node -> b);
        break;
        
    default:
        break;
    }
    



}
