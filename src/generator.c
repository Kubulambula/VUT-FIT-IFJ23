#include "error.h"
#include "ast.h"
#include "lexer.h"  
#include <stdlib.h>

// built-in funcs detection
// func processing
//need to create stack for frames ?






void func_built_in(){

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
    printf("RETURN \n");

    //FUNC substring FOR TESTING
    /*
    LABEL substring
    CREATEFRAME
    #definice promenych
    DEFVAR TF@retV
    DEFVAR TF@&1
    DEFVAR TF@&2
    DEFVAR TF@&3
    MOVE TF@&1 GF@arg           #retezec
    MOVE TF@&2 GF@arg2          #startIndex i
    MOVE TF@&3 GF@arg3          #endIndex+1 j
    DEFVAR TF@&temp             #promena pro pocet cyklu
    DEFVAR TF@&tempBool         #pomocna promena pro skoky
    DEFVAR TF@&tempChar         #pomocna promena pro ukladani znaku
    SUB TF@&temp TF@&2 TF@&1    #pocet cyklu
    
    #cyklus
    LABEL substringWHILE
    LT TF@&tempBool TF@&temp TF@&2
    JUMPIFEQ substringEND TF@tempBool bool@false
        GETCHAR tempChar TF@&1 TF@tempBool
        CONCAT TF@retV TF@retV tempChar
        ADD TF@tempBool TF@tempBool int@1
        JUMP substringWHILE
    LABEL substringEND

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

}


