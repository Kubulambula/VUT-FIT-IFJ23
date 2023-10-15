#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "buffer_string.h"
#include "lexer.h"

bool DELIMITER = 0;
bool BLANK =0;
FILE* g_file; 

typedef enum{
    // Error token
    TOKEN_ERR, // token indicating lexical error (ERR_LEXICAL)
    // Control tokens
    TOKEN_EOL, // ned of line / new line / linefeed / whatever you call it
    TOKEN_EOF, // end of file
    // Keywords
    TOKEN_KEYWORD_NIL, // nil
    TOKEN_KEYWORD_INT, // Int
    TOKEN_KEYWORD_DOUBLE, // Double
    TOKEN_KEYWORD_STRING, // String
    TOKEN_KEYWORD_VAR, // var
    TOKEN_KEYWORD_LET, // let
    TOKEN_KEYWORD_IF, // if
    TOKEN_KEYWORD_ELSE, // else
    TOKEN_KEYWORD_WHILE, // while
    TOKEN_KEYWORD_FUNC, // func
    TOKEN_KEYWORD_RETURN, // return
    // Parentheses
    TOKEN_PARENTHESIS_LEFT, // (
    TOKEN_PARENTHESIS_RIGHT, // )
    // Braces
    TOKEN_BRACE_LEFT, // {
    TOKEN_BRACE_RIGHT, // }
    // Aritmethic operators
    TOKEN_OPERATOR_PLUS, // +
    TOKEN_OPERATOR_MINUS, // -
    TOKEN_OPERATOR_MULTIPLICATION, // *
    TOKEN_OPERATOR_DIVISION, // /
    // Comparison operatos
    TOKEN_OPERATOR_LESS_THAN, // <
    TOKEN_OPERATOR_GREATER_THAN, // >
    TOKEN_OPERATOR_LESS_THAN_OR_EQUAL, // <=
    TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL, // >=
    TOKEN_OPERATOR_EQUALS, // ==
    TOKEN_OPERATOR_NOT_EQUALS, // !=
    // Literals
    TOKEN_LITEREAL_INT, // an int literal
    TOKEN_LITEREAL_DOUBLE, // a double literal
    TOKEN_LITEREAL_STRING, // a string literal
    // Miscellaneous 
    TOKEN_ASSIGN, // =
    TOKEN_EXCLAMATION, // ! - after identifier it's OPERATOR_FORCED_UNWRAPPING and before it's OPERATOR_NOT
    TOKEN_QUESTION, // ?
    TOKEN_NIL_COALESCING, // ??
    TOKEN_COMMA, // ,
    TOKEN_COLON, // :
    TOKEN_ARROW, // ->
    // Identifier
    TOKEN_IDENTIFIER, // variable / function identifier
} Token;




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
    } while(isblank(*whiteSpace));
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


//
bool is_keyword(char *str, int *TokenType){
    // List of C keywords
    int offset = 4;

    const char *keywords[] = {
        "nil", "Int", "Double", "String", "var", "let", "if", "else",
        "while", "func", "return"
    };

    // Calculate the number of keywords
    size_t numKeywords = sizeof(keywords) / sizeof(keywords[0]);

    // Check if the input string matches any keyword
    for (size_t i = 0; i < numKeywords; ++i) {
        if (strcmp(str, keywords[i]) == 0) {
            *TokenType = i + offset;
            return true;  // Its a keyword
        }
    }

    return false;  // Not a keyword
}

//TO DO 
bool is_double(){

}

//TO DO
bool is_int(){

}

//TO DO
bool is_string(){

}

//TO DO
bool is_identifier(){

}

// Delimeter functions checks if theres another token ahead
bool is_delimiter(char Char){
    // List of common delimiters, add more as needed
    const char delimiters[] =   {'>', '\t', '\n', '<', ';', '(', ')', '{', '}', '[', ']', '=', '/', '*', '+', '-', 
                                '!', '?'};

    // Check if the character is a delimiter
    for (size_t i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]); ++i) {
        if (Char == delimiters[i]) {
            return true;  // It's a delimiter
        }
    }

    return false;  // Not a delimiter
}

//TO DO
bool is_Miscellaneous(char *str, int *TokenType){

}

//TO DO
bool is_aritmethic_operator(char *str, int *TokenType){

}

//TO DO
bool is_comparison_operator(char Char, int *TokenType){

} 

// Funkce najde a vytvori token
Token get_next_token(BufferString* buffer_string){    
    
    //TO DO, example 'a+b*c' or comparison
    if(DELIMITER==1){
    /*
        is_aritmethic_operator();
        is_comparison_operator();
        is_Miscellaneous();
    */
    }


    int tokenType = 0;
    char nextChar = get_next_char();
    Token token;
    
    //End of the file, nothing to do
    if(nextChar == EOF){
        //should still return token
        exit;
    }

    //White space
    if(isblank(nextChar)){
        skip_white_space(nextChar);
    }

    //Comment
    if(nextChar == '/'){
        nextChar = get_next_char();
        if(nextChar == '*'){skip_comments_ML();}
        if(nextChar == '/'){skip_comments_SL();}
    }



    // TOKEN STARTS WITH NUMBER
    if(isalnum(nextChar)){
        do {
            nextChar = get_next_char();
            if(nextChar == EOF){exit;}

            buffer_string->string[buffer_string->length]=nextChar;
            buffer_string_append_char(buffer_string,nextChar);
            buffer_string->length++;
            buffer_string->alloc_length++;
        
        } while(!isblank(nextChar) && !is_delimiter(nextChar));
        

    }

    // TOKEN STARTS WITH A LETTER
    if(isalpha(nextChar)){
        do {
            nextChar = get_next_char();
            if(nextChar == EOF){exit;}

            buffer_string->string[buffer_string->length]=nextChar;
            buffer_string_append_char(buffer_string,nextChar);
            buffer_string->length++;
            buffer_string->alloc_length++;
        
        } while(!isblank(nextChar));

        if(is_keyword(buffer_string->string,tokenType)){
            token = tokenType;
            return;
        }
    }   

    // TO DO, could be increment or assign after white space
    if(is_delimiter(nextChar)){


    }

};

/* TESTING
int main(int argc, char* argv[]) {
	initLexer("test");
    BufferString* buffer_string;
    get_next_token(buffer_string);
	return OK;
}
*/
