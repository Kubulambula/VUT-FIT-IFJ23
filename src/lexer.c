#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "buffer_string.h"
#include "lexer.h"

FILE* source_file; 

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

typedef enum {
    // Start state
    LEXER_STATE_START, // starting state
    LEXER_STATE_POSSIBLE_COMMENT, // Leads from '/' to -> '//' '*' 
    // Identifier, possible keyword
    LEXER_STATE_IDENTIFIER_OR_KEYWORD, // State from 'LEXER_STATE_START' -> 'LEXER_STATE_INDETIFIER_OR_KEYWORD'
    // Literals
    LEXER_STATE_INT, // State from 'LEXER_STATE_START' -> 'LEXER_STATE_INT'
    LEXER_STATE_DOUBLE, // State from 'LEXER_STATE_INT' -> 'LEXER_STATE_DOUBLE'
    

    //States to check double char operators and such
    //All start from 'LEXER_STATE_START'
    LEXER_STATE_STRING, // Leads from '"' to anything until '"'
    LEXER_STATE_MINUS_OR_ARROW, // Leads from '-' -> '->'
    LEXER_STATE_GREATER_POSSIBLE_EQUAL, // Leads from '>' -> '>='
    LEXER_STATE_LESS_POSSIBLE_EQUAL, // Leads from '<' -> '<='
    LEXER_STATE_POSSIBLE_EQUALS, // Leads from '=' -> '=='
    LEXER_STATE_POSSIBLE_NOT_EQUALS, // Leads from '!' -> '!='
    LEXER_STATE_POSSIBLE_COALESCING, // Leads from '?' -> '??'




    // End of a file
    LEXER_STATE_EOF,
    // Invalide character
    LEXER_STATE_INVALID_CHARACTER,
} State;


void initLexer(FILE* file){
    if (file == NULL){
        return;
    }
    source_file = file;
};


//
char get_next_char(){
    char Nextchar = getc(source_file);
    //printf("%C",Nextchar);
    return Nextchar;
};

void skip_white_space(char whiteSpace){
    while(isblank(whiteSpace)) {
        whiteSpace = get_next_char();
    }
    ungetc(source_file,whiteSpace);
}

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
    ungetc(Char,source_file); // to read '\n' again
};


// Handle keyword matching
bool is_keyword(BufferString* buffer_string, Token* TokenType){


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
        if (buffer_string_cmp_str(buffer_string,keywords[i]) == 0) {
            *TokenType = i + offset;
            return true;  // Its a keyword
        }
    }

    return false;  // Not a keyword
}

/*
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
*/

// Funkce najde a vytvori token
Token get_next_token(BufferString* buffer_string){    

        char nextChar;
        int tokenType = 0;
        Token token;
        State state = LEXER_STATE_START;

        while(1){

            nextChar = get_next_char();

            switch (state) {
                case LEXER_STATE_START:
                    //else if could be better?

                    //White space
                    if(isblank(nextChar)){
                        skip_white_space(nextChar);
                    }
                    
                    //Comment
                    if(nextChar == '/'){
                        state = LEXER_STATE_POSSIBLE_COMMENT;
                    }
                    
                    //END OF FILE
                    if(nextChar == EOF){
                        state = LEXER_STATE_EOF;
                    }

                    //END OF LINE
                    if(nextChar == '\n'){
                        token = TOKEN_EOL;
                        return token;
                    }


                    //Should append here or use ungetc?
                    //token is int or double
                    if(isalnum(nextChar)){
                        state = LEXER_STATE_INT;
                        ungetc(nextChar,source_file); //"unreading" char to get read it again for next state?  
                    }

                    //token is keyword or identifier
                    if(isalpha(nextChar) || nextChar=='_'){
                        state = LEXER_STATE_IDENTIFIER_OR_KEYWORD;
                        ungetc(nextChar,source_file); //"unreading" char to get read it again for next state?
                    }    
                    

                    //token is string
                    if(nextChar == '"'){
                        state = LEXER_STATE_STRING;
                        buffer_string_append_char(nextChar); // append the first '"'
                        // ungetc(nextChar,source_file); //"unreading" char to get read it again for next state?
                    }
                    //--------------------------

                    //token is plus
                    if(nextChar == '+'){
                        token = TOKEN_OPERATOR_PLUS;
                        return token;
                    }

                    //token is multiplication
                    if(nextChar == '*'){
                        token = TOKEN_OPERATOR_MULTIPLICATION;
                        return token;
                    }

                    //token is comma
                    if(nextChar == ','){
                        token = TOKEN_COMMA;
                        return token;
                    }

                    //token is colon
                    if(nextChar ==':'){
                        token = TOKEN_COLON;
                        return token;
                    }

                    //token could be minus or arrow
                    if(nextChar =='-'){
                        state = LEXER_STATE_MINUS_OR_ARROW;
                    }

                    //token could be "less than" or "less than or equal"
                    if(nextChar == '<'){
                        state = LEXER_STATE_LESS_POSSIBLE_EQUAL;
                    }

                    //token could be "more than" or "more than or equal"
                    if(nextChar == '>'){
                        state = LEXER_STATE_GREATER_POSSIBLE_EQUAL;
                    }

                    //token could be "EXCLAMATION" or "not equal"
                    if(nextChar == '!'){
                        state=LEXER_STATE_POSSIBLE_NOT_EQUALS;
                    }

                    //token could be "assign" or "equal"
                    if(nextChar == '='){
                        state = LEXER_STATE_GREATER_POSSIBLE_EQUAL;
                    }

                    //token could be "question" or "coalescing"
                    if(nextChar == '?'){
                        state = LEXER_STATE_POSSIBLE_COALESCING;
                    }

                    //token is left parenthesis
                    if(nextChar == '('){
                        token = TOKEN_PARENTHESIS_LEFT;
                        return token;
                    }

                    //token is right parenthesis
                    if(nextChar == ')'){
                        token = TOKEN_PARENTHESIS_RIGHT;
                        return token;
                    }

                    //token is left brace
                    if(nextChar == '{'){
                        token = TOKEN_BRACE_LEFT;
                        return token;
                    }

                    //token is right brace
                    if(nextChar == '}'){
                        token = TOKEN_BRACE_RIGHT;
                        return token;
                    }

                    //Possible for white space to fall through start state
                    //Character cannot be processed
                    if(state == LEXER_STATE_START && !isblank(nextChar)){
                        state = LEXER_STATE_INVALID_CHARACTER;
                    }

                    break;
                
                // STATES
                case LEXER_STATE_EOF:
                    token = TOKEN_EOF;
                    return token;
                    break;

                case LEXER_STATE_INVALID_CHARACTER:
                    token = TOKEN_ERR;
                    return token;
                    break;

                case LEXER_STATE_POSSIBLE_COMMENT:
                    if(nextChar == '*'){
                        skip_comments_ML();
                    }   
                    else if(nextChar == '/'){
                        skip_comments_SL();
                    }
                    else {
                        ungetc(nextChar,source_file);
                        token = TOKEN_OPERATOR_DIVISION;
                        return token;
                    }
                    break;
                
                case LEXER_STATE_INT:
                    if (isalnum(nextChar)){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else{
                        if( nextChar == '.' || nextChar=='e' || nextChar=='E'){
                            buffer_string_append_char(buffer_string, nextChar);
                            state = LEXER_STATE_DOUBLE;
                        } else{
                            token = TOKEN_LITEREAL_INT;
                            buffer_string_append_char(buffer_string, '\0');
                            return token;
                        }
                    }
                    break;

                case LEXER_STATE_DOUBLE:
                    if (isalnum(nextChar)){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else{
                        ungetc(nextChar,source_file);
                        token = TOKEN_LITEREAL_DOUBLE;
                        buffer_string_append_char(buffer_string, '\0');
                        return token;
                    }
                    break;

                case LEXER_STATE_IDENTIFIER_OR_KEYWORD:
                    //Flag for num, cant be keyword?
                    if(isalnum(nextChar) || isalpha(nextChar) || nextChar=='_'){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else{
                        ungetc(nextChar,source_file); // to read unknown char again for the start state
                        if(is_keyword(buffer_string,tokenType)){
                            token = tokenType;
                        } else {
                            token = TOKEN_IDENTIFIER;
                        }
                        buffer_string_append_char(buffer_string, '\0');
                        return token;
                    }

                    break;

                case LEXER_STATE_STRING:
                    if(nextChar != '"'){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else{
                        buffer_string_append_char(buffer_string, nextChar); // apppend the second '"'
                        buffer_string_append_char(buffer_string, '\0'); // append '\0' symbol
                        token = TOKEN_LITEREAL_STRING;
                        return token;
                    }
                    break;

                
                case LEXER_STATE_MINUS_OR_ARROW:
                    if(nextChar == '>'){
                        token = TOKEN_ARROW;
                        return token;
                    } else {
                        ungetc(nextChar,source_file);
                        token = TOKEN_OPERATOR_MINUS;
                        return token;
                    }
                    break;

                // could be single state somehow ?
                case LEXER_STATE_LESS_POSSIBLE_EQUAL:
                    if(nextChar == '='){
                        token = TOKEN_OPERATOR_LESS_THAN_OR_EQUAL;
                        return token;
                    } else {
                        ungetc(nextChar,source_file);
                        token = TOKEN_OPERATOR_LESS_THAN;
                        return token;
                    }
                    break;

                case LEXER_STATE_GREATER_POSSIBLE_EQUAL:
                    if(nextChar == '='){
                        token = TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL;
                        return token;
                    } else {
                        ungetc(nextChar,source_file);
                        token = TOKEN_OPERATOR_GREATER_THAN;
                        return token;
                    }
                    break;
                    
                case LEXER_STATE_POSSIBLE_NOT_EQUALS:
                    if(nextChar == '='){
                        token = TOKEN_OPERATOR_NOT_EQUALS;
                        return token;
                    } else {
                        ungetc(nextChar,source_file);
                        token = TOKEN_EXCLAMATION;
                        return token;
                    }
                    break;

                case LEXER_STATE_POSSIBLE_EQUALS:
                    if(nextChar == '='){
                        token = TOKEN_OPERATOR_EQUALS;
                        return token;
                    } else {
                        ungetc(nextChar,source_file);
                        token = TOKEN_ASSIGN;
                        return token;
                    }
                    break;
                // -----------------------------------------------------
                
                case LEXER_STATE_POSSIBLE_COALESCING:
                    if(nextChar == '?'){
                        token = TOKEN_QUESTION;
                        return token;
                    } else {
                        ungetc(nextChar,source_file);
                        token = TOKEN_NIL_COALESCING;
                        return token;
                    }                    
                    break;
                default:
                    break;
        }
    }

}

