#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "buffer_string.h"
#include "lexer.h"

FILE* source_file; 



void initLexer(FILE* file){
    if (file == NULL){
        return;
    }
    source_file = file;
};


//
inline char get_next_char(){
#ifdef NDEBUG
    return  getc(source_file);
#else
    char Nextchar = getc(source_file);
    printf("%C",Nextchar);
    return Nextchar;
#endif
};

void skip_white_space(char whiteSpace){
    while(isblank(whiteSpace)) {
        whiteSpace = get_next_char();
    }
    ungetc(whiteSpace, source_file);
}

// Handle comments mutli line
void skip_comments_ML(){
    char Char = '\0';
    char prevChar;
    while(1){
        prevChar = Char;
        Char = get_next_char();
        if(prevChar == '*' && Char == '/'){return;}
    }
};

// Handle comments single line
void skip_comments_SL(){
    char Char = get_next_char();
    while(Char != '\n'){
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
                    else if(nextChar == '/'){
                        state = LEXER_STATE_POSSIBLE_COMMENT;
                    }
                    
                    //END OF FILE
                    else if(nextChar == EOF){
                        state = LEXER_STATE_EOF;
                    }

                    //END OF LINE
                    else if(nextChar == '\n'){
                        return TOKEN_EOL;
                        
                    }


                    //Should append here or use ungetc?
                    //token is int or double
                    else if(isdigit(nextChar)){
                        state = LEXER_STATE_INT;
                        ungetc(nextChar,source_file); //"unreading" char to get read it again for next state?  
                    }

                    //token is keyword or identifier
                    else if(isalpha(nextChar) || nextChar=='_'){
                        state = LEXER_STATE_IDENTIFIER_OR_KEYWORD;
                        ungetc(nextChar,source_file); //"unreading" char to get read it again for next state?
                    }    
                    

                    //token is string
                    else if(nextChar == '"'){
                        state = LEXER_STATE_STRING;
                        // ungetc(nextChar,source_file); //"unreading" char to get read it again for next state?
                    }
                    //--------------------------

                    //token is plus
                    else if(nextChar == '+'){
                        return TOKEN_OPERATOR_PLUS;
                        
                    }

                    //token is multiplication
                    else if(nextChar == '*'){
                        return TOKEN_OPERATOR_MULTIPLICATION;
                        
                    }

                    //token is comma
                    else if(nextChar == ','){
                        return TOKEN_COMMA;
                        
                    }

                    //token is colon
                    else if(nextChar ==':'){
                        return TOKEN_COLON;
                        
                    }

                    //token could be minus or arrow
                    else if(nextChar =='-'){
                        state = LEXER_STATE_MINUS_OR_ARROW;
                    }

                    //token could be "less than" or "less than or equal"
                    else if(nextChar == '<'){
                        state = LEXER_STATE_LESS_POSSIBLE_EQUAL;
                    }

                    //token could be "more than" or "more than or equal"
                    else if(nextChar == '>'){
                        state = LEXER_STATE_GREATER_POSSIBLE_EQUAL;
                    }

                    //token could be "EXCLAMATION" or "not equal"
                    else if(nextChar == '!'){
                        state=LEXER_STATE_POSSIBLE_NOT_EQUALS;
                    }

                    //token could be "assign" or "equal"
                    else if(nextChar == '='){
                        state = LEXER_STATE_GREATER_POSSIBLE_EQUAL;
                    }

                    //token could be "question" or "coalescing"
                    else if(nextChar == '?'){
                        state = LEXER_STATE_POSSIBLE_COALESCING;
                    }

                    //token is left parenthesis
                    else if(nextChar == '('){
                        return TOKEN_PARENTHESIS_LEFT;
                        
                    }

                    //token is right parenthesis
                    else if(nextChar == ')'){
                        return TOKEN_PARENTHESIS_RIGHT;
                        
                    }

                    //token is left brace
                    else if(nextChar == '{'){
                        return TOKEN_BRACE_LEFT;
                        
                    }

                    //token is right brace
                    else if(nextChar == '}'){
                        return TOKEN_BRACE_RIGHT;
                        
                    }

                    //Possible for white space to fall through start state
                    //Character cannot be processed
                    else {
                        state = LEXER_STATE_INVALID_CHARACTER;
                    }

                    break;
                
                // STATES
                case LEXER_STATE_EOF:
                    return TOKEN_EOF;
                    
                    break;

                case LEXER_STATE_INVALID_CHARACTER:
                    return TOKEN_ERR;
                    
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
                        return TOKEN_OPERATOR_DIVISION;
                        
                    }
                    break;
                
                case LEXER_STATE_INT:
                    if (isdigit(nextChar)){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else if( nextChar == '.' || nextChar=='e' || nextChar=='E'){
                            buffer_string_append_char(buffer_string, nextChar);
                            state = LEXER_STATE_DOUBLE;
                    } else{
                            return TOKEN_LITEREAL_INT;
                            
                    }
                    
                    break;

                case LEXER_STATE_DOUBLE:
                    if (isdigit(nextChar)){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else{
                        ungetc(nextChar,source_file);
                        return TOKEN_LITEREAL_DOUBLE;
                        
                    }
                    break;

                case LEXER_STATE_IDENTIFIER_OR_KEYWORD:
                    //Flag for num, cant be keyword?
                    if(isalnum(nextChar) || nextChar=='_'){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else{
                        ungetc(nextChar,source_file); // to read unknown char again for the start state
                        if(is_keyword(buffer_string,tokenType)){
                            return tokenType;
                        } else {
                            return TOKEN_IDENTIFIER;
                        }
                        
                    }

                    break;

                case LEXER_STATE_STRING:
                    bool bufferEmpty = buffer_string_get_length(buffer_string);
                    if(nextChar != '"'){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else if(!bufferEmpty){
                        return TOKEN_LITEREAL_STRING;
                        
                    } else {
                        state = LEXER_STATE_POSSIBLE_MULTI_STRING;
                    }
                    break;

                
                case LEXER_STATE_MINUS_OR_ARROW:
                    if(nextChar == '>'){
                        return TOKEN_ARROW;
                        
                    } else {
                        ungetc(nextChar,source_file);
                        return TOKEN_OPERATOR_MINUS;
                        
                    }
                    break;

                // could be single state somehow ?
                case LEXER_STATE_LESS_POSSIBLE_EQUAL:
                    if(nextChar == '='){
                        return TOKEN_OPERATOR_LESS_THAN_OR_EQUAL;
                        
                    } else {
                        ungetc(nextChar,source_file);
                        return TOKEN_OPERATOR_LESS_THAN;
                        
                    }
                    break;

                case LEXER_STATE_GREATER_POSSIBLE_EQUAL:
                    if(nextChar == '='){
                        return TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL;
                        
                    } else {
                        ungetc(nextChar,source_file);
                        return TOKEN_OPERATOR_GREATER_THAN;
                        
                    }
                    break;
                    
                case LEXER_STATE_POSSIBLE_NOT_EQUALS:
                    if(nextChar == '='){
                        return TOKEN_OPERATOR_NOT_EQUALS;
                        
                    } else {
                        ungetc(nextChar,source_file);
                        return TOKEN_EXCLAMATION;
                        
                    }
                    break;

                case LEXER_STATE_POSSIBLE_EQUALS:
                    if(nextChar == '='){
                        return TOKEN_OPERATOR_EQUALS;
                        
                    } else {
                        ungetc(nextChar,source_file);
                        return TOKEN_ASSIGN;
                        
                    }
                    break;
                // -----------------------------------------------------
                
                case LEXER_STATE_POSSIBLE_COALESCING:
                    if(nextChar == '?'){
                        return TOKEN_QUESTION;
                        
                    } else {
                        ungetc(nextChar,source_file);
                        return TOKEN_NIL_COALESCING;
                        
                    }                    
                    break;

                case LEXER_STATE_POSSIBLE_MULTI_STRING:
                    if (nextChar == '"'){
                        state = LEXER_STATE_MULTI_STRING;
                    } else {
                        ungetc(nextChar,source_file);
                        state = LEXER_STATE_STRING; 
                    }
                    break;
                case LEXER_STATE_MULTI_STRING:
                    if(nextChar != '"'){
                        buffer_string_append_char(buffer_string, nextChar);
                    } else {
                        return TOKEN_LITEREAL_MULTILINE_STRING;
                    }
                    break;
                default:
                    break;
        }
    }

}

