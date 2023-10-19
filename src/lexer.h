#ifndef LEXER_H
#define LEXER_H

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
    TOKEN_LITEREAL_MULTILINE_STRING, // a multiline string literal
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
    LEXER_STATE_POSSIBLE_MULTI_STRING, // Leads from " -> ""
    LEXER_STATE_MULTI_STRING, // Leads from "" -> """
    // End of a file
    LEXER_STATE_EOF,
    // Invalide character
    LEXER_STATE_INVALID_CHARACTER,
} State;



// gets next char
char get_next_char();

// sets source_file
void initLexer(FILE* file);

//skips white space
void skip_white_space(char whiteSpace);

// skips single line comment
// TODO should still return '\n' as token
void skip_comments_SL();

// skips multiline comments
void skip_comments_ML();

// checks if loaded buffer_string is keyword 
bool is_keyword(BufferString* buffer_string, Token* TokenType);

// finds next token
// returns Token type
Token get_next_token(BufferString* buffer_string);

#endif