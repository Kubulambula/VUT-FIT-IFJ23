#include "error.h"
#include "ast.h"
#include "lexer.h"  
#include "symtable.h"
#include "buffer_string.h"  
#include <stdlib.h>

// built-in funcs detection
// func processing
//need to create stack for frames ?



void code_generation_buildin(){

    // ----- READ FUNC -----

    // func readString
    printf("LABEL readString\n"); // Jmeno funkce
    printf("CREATEFRAME\n");
    printf("PUSHFRAME \n"); // Lokalni ramec
    printf("DEFVAR LF@valS"); // promena do ktere se bude cist
    printf("READ LF@valS string\n"); // cteni promene
    printf("POPFRAME \n"); // Hodi to do TF
    printf("RETURN \n"); // vrati se a muze stim pracovat v TF ?

    // func readInt
    printf("LABEL readInt\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME \n");
    printf("DEFVAR LF@valI"); // promena do ktere se bude cist
    printf("READ LF@valI string\n"); // cteni promene
    printf("POPFRAME \n");
    printf("RETURN \n");

    // func readDouble
    printf("LABEL readDouble\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME \n");
    printf("DEFVAR LF@valD"); // promena do ktere se bude cist
    printf("READ LF@valD string\n"); // cteni promene
    printf("POPFRAME \n");
    printf("RETURN \n");    
    
    //GENERIC READ FUNC FOR TESTING
    /*
    LABEL readString
    CREATEFRAME
    PUSHFRAME
    DEFVAR LF@valS
    READ LF@valS string
    POPFRAME
    RETURN 
    */

    // ----- CONVERSION FUNCS -----

    // func Int2Double
    printf("LABEL Int2Double\n"); 
    printf("CREATEFRAME\n"); // vytvoreni TF pro tuhle srandu honestly not sure jestli to bude fungovat
    printf("DEFVAR TF@&retV"); // tady bude hodnota typu double
    printf("DEFVAR TF@&1"); // tady bude hodnota typu int, nakopiruje se sem argument
    printf("MOVE TF@&2 GF@arg"); //generic arg, argument funkce co volam proste
    printf("INT2FLOAT TF@&retV TF@&1\n"); // prevod
    printf("RETURN \n");

    // func Double2Int
    printf("LABEL Double2Int\n");
    printf("CREATEFRAME\n"); // vytvoreni TF pro tuhle srandu honestly not sure jestli to bude fungovat
    printf("DEFVAR TF@&retV"); // tady bude hodnota typu int
    printf("DEFVAR TF@&1"); // tady bude hodnota typu double, nakopiruje se sem argument
    printf("MOVE TF@&1 GF@arg"); //generic arg, argument funkce co volam proste
    printf("FLOAT2INT TF@&retV TF@&1\n"); // prevod
    printf("RETURN \n");
    
    //GENERIC CONVERSION FUNC FOR TESTING
    /*
    LABEL Double2Int
    CREATEFRAME
    DEFVAR TF@&retV
    DEFVAR TF@&1
    MOVE TF@&1 GF@arg
    FLOAT2INT TF@&retV TF@&1
    RETURN 
    */


    // ----- SPECIFIC FUNCS -----
    // func length
    printf("LABEL length\n");
    printf("CREATEFRAME\n"); // vytvoreni TF pro tuhle srandu honestly not sure jestli to bude fungovat
    printf("DEFVAR TF@&retV"); // tady bude hodnota typu int
    printf("DEFVAR TF@&1"); // tady bude hodnota typu string, nakopiruje se sem argument
    printf("MOVE TF@&1 GF@arg"); //generic arg, argument funkce co volam proste
    printf("STRLEN TF@&retV TF@&1"); //delka string ulozena do &1
    printf("RETURN \n");

    //FUNC length FOR TESTING
    /*
    LABEL length
    CREATEFRAME
    DEFVAR TF@&retV
    DEFVAR TF@&1
    MOVE TF@&1 GF@arg
    STRLEN TF@&retV TF@&1
    RETURN
    */

    // func substring
    printf("LABEL substring\n");
    printf("CREATEFRAME\n");
    //definice promenych
    printf("DEFVAR TF@&retV\n");
    printf("DEFVAR TF@&1\n");
    printf("DEFVAR TF@&2\n");
    printf("DEFVAR TF@&3\n");
    printf("MOVE TF@&retV string@\n");
    printf("MOVE TF@&1 GF@arg\n");           //retezec
    printf("MOVE TF@&2 GF@arg2\n");          //startIndex i
    printf("MOVE TF@&3 GF@arg3\n");          //endIndex+1 j
    printf("DEFVAR TF@tempBool\n");         //pomocna promena pro skoky
    printf("DEFVAR TF@tempChar\n");         //pomocna promena pro ukladani znaku
    
    //cyklus
    printf("LABEL substringWHILE\n");
    printf("LT TF@tempBool TF@&2 TF@&3\n"); 
    printf("JUMPIFEQ substringEND TF@tempBool bool@false\n");
        printf("GETCHAR TF@tempChar TF@&1 TF@&2\n");
        printf("CONCAT TF@&retV TF@&retV TF@tempChar\n");
        printf("ADD TF@&2 TF@&2 int@1\n");
        printf("JUMP substringWHILE\n");
    printf("LABEL substringEND\n");
    printf("RETURN \n");

    //FUNC substring FOR TESTING
    /*
    LABEL substring
    CREATEFRAME
    #definice promenych
    DEFVAR TF@&retV
    DEFVAR TF@&1
    DEFVAR TF@&2
    DEFVAR TF@&3
    MOVE TF@&retV string@
    MOVE TF@&1 GF@arg           #retezec
    MOVE TF@&2 GF@arg2          #startIndex i
    MOVE TF@&3 GF@arg3          #endIndex+1 j
    DEFVAR TF@tempBool         #pomocna promena pro skoky
    DEFVAR TF@tempChar         #pomocna promena pro ukladani znaku
    
    #cyklus
    LABEL substringWHILE
    LT TF@tempBool TF@&2 TF@&3 
    JUMPIFEQ substringEND TF@tempBool bool@false
        GETCHAR TF@tempChar TF@&1 TF@&2
        CONCAT TF@&retV TF@&retV TF@tempChar
        ADD TF@&2 TF@&2 int@1
        JUMP substringWHILE
    LABEL substringEND

    RETURN

    RETURN
    */

    // func ord
    printf("LABEL ord\n");
    printf("CREATEFRAME\n"); 
    printf("DEFVAR TF@&retV"); // tady bude hodnota typu int
    printf("DEFVAR TF@&1"); // tady bude hodnota typu string, nakopiruje se sem argument
    printf("MOVE TF@&1 GF@arg"); //generic arg, argument funkce co volam proste
    
    //Kontrola retezce
    printf("DEFVAR TF@temp"); // tady bude hodnota typu int
    printf("DEFVAR TF@tempBool"); // tady bude hodnota typu int
    printf("STRLEN TF@temp TF@&1"); // kontrola delky, slo by pouzit getchar mby? nebo i xor teoreticky neco takoveho
    printf("EQ TF@tempBool TF@temp int@0");
    printf("JUMPIFEQ ordCNT TF@tempBool bool@false");

    //EMPTY STRING
    printf("MOVE TF@&retV int@0"); 
    printf("RETURN \n");
    
    //NOT-EMPTY STRING
    printf("LABEL ordCNT\n"); // co kdyz tu budu mit user func odr1 ? musi se osefit nebo skip
    printf("STRI2INT TF@&retV TF@&2 int@0"); //hodnota ASCII ulozena do &1
    printf("RETURN \n");

    //FUNC ord FOR TESTING
    /*
    LABEL ord
    CREATEFRAME
    DEFVAR TF@&retV
    DEFVAR TF@&1
    MOVE TF@&1 GF@arg

    DEFVAR TF@temp
    DEFVAR TF@tempBool
    STRLEN TF@temp TF@&1
    EQ TF@tempBool TF@temp int@0
    JUMPIFEQ ordCNT TF@tempBool bool@false

    MOVE TF@&retV int@0
    RETURN

    LABEL ordCNT
    STRI2INT TF@&retV TF@&2 int@0
    RETURN
    */


    // func chr 
    printf("LABEL chr\n"); 
    printf("CREATEFRAME\n"); 
    printf("DEFVAR TF@&retV"); // tady bude hodnota typu char
    printf("DEFVAR TF@&1"); // tady bude hodnota typu int, nakopiruje se sem argument
    printf("MOVE TF@&1 GF@arg"); //generic arg, argument funkce co volam proste
    printf("INT2CHAR TF@&retV TF@&1");
    printf("RETURN \n");

    //FUNC chr FOR TESTING
    /*
    LABEL chr
    CREATEFRAME
    DEFVAR TF@&retV
    DEFVAR TF@&1
    MOVE TF@&1 GF@arg
    INT2CHAR TF@&retV TF@&1
    RETURN
    */
   


    // func write
    printf("LABEL write\n");
    printf("RETURN \n");

}


void generate_args(FuncDefArg* Arg){
    int ArgNum = 1;
    while(Arg->next != NULL){
        printf("DEFVAR TF@&%d",ArgNum);
        printf("MOVE TF@&%d GF@arg%d",ArgNum, ArgNum);  // this should be done when calling the func
        ArgNum++;
    }
}


// suboptimal implementation 
char* process_string_literal(char* string){
    BufferString IFJ23string;
    BufferString_init(&IFJ23string);
    int Idx = 0;
    while(string[Idx]!='\0'){ // strlen better ?
        if(string[Idx] <= ' ' || string[Idx] == '#' || string[Idx] == '\\'){
            char asciiValue[6]; // Assuming ASCII values are 3 digits or less
            sprintf(asciiValue, "\\%03d", string[Idx]);
            BufferString_append_str(&IFJ23string, asciiValue);
        } else{
            BufferString_append_char(&IFJ23string, string[Idx]);
        }
        Idx++;
    }
    char *IFJ23stringOut = BufferString_get_as_string(&IFJ23string);
    return IFJ23stringOut; // has to free in main body, do we care if it works?
}



/*
// might not be necessary
// This needs to be done so retV can be assigned value beforehand
void generate_retV(Type type){
    switch (type)
    {
    //TODO
    //Types ...
    case STRING:
        printf("DEFVAR TF@&retV");
        printf("MOVE TF@&retV string@\n");
        break;
    
    default:
        break;
    }

}
*/


void code_generation_prep(){
    printf("LABEL main");
    code_generation_buildin();
    printf("JUMP MAIN");
}

void code_generation(ASTNode* node){

    //ASTNode* nextNode ;

    switch (node -> type)
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
        printf("LABEL %s", node -> a);
        printf("CREATEFRAME");
        if(node->b != NULL)
            generate_args(node -> b);
        break;
        
    default:
        break;
    }
    



}








