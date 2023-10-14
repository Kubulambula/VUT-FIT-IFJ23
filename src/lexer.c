#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "buffer_string.h"
#include "lexer.h"

#define Identifier 1
#define Operator 2
#define Constant 3
#define Keyword 4
#define Special_Char 5


FILE* g_file; 





void initLexer(char* filename){
    //printf("%s",filename);
    g_file = fopen(filename, "r");
    if (g_file == NULL) {
        perror("Error opening file for reading");
        exit(1); 
    }
};


//
char get_next_char(){
    char Nextchar = getc(g_file);
    //printf("%C",Nextchar);
    return Nextchar;
};

// Handle whitespaces
void skip_white_space(char* whiteSpace){
    do{
        *whiteSpace = get_next_char();
    } while(*whiteSpace == ' ');
};

// Handle comments mutli line
void skip_comments_ML(){
    char Char;
    char prevChar;
    while(1){
        prevChar = Char;
        Char = get_next_char();
        if(prevChar == '*' && Char == "/"){return;}
    }
};

// Handle comments single line
void skip_comments_SL(){
    char Char = get_next_char();
    while(Char != "\n"){
        Char = get_next_char();
    }
};

// Funkce najde a vytvori token
void get_next_token(/*BufferString* id, Token* token*/){
    char nextChar = get_next_char();
    if(nextChar == EOF){
        exit;
    }

    if(nextChar == ' '){
        skip_white_space(nextChar);
    }

    if(nextChar == '/'){
        nextChar = get_next_char();
        if(nextChar == '*'){skip_comments_ML();}
        if(nextChar == '/'){skip_comments_SL();}
    }



};

int main(int argc, char* argv[]) {
	initLexer("test");
    get_next_token();
	return OK;
}
