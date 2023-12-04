#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdbool.h>
#include "buffer_string.h"
#include "error.h"


typedef enum{
    // Error token
    TOKEN_ERR_INTERNAL, // token indicating internal error (ERR_INTERNAL)
    TOKEN_ERR_LEXICAL, // token indicating lexical error (ERR_LEXICAL)
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
    TOKEN_CONCATENATE, // internal token - concatenation
    TOKEN_OPERATOR_MINUS, // -
    TOKEN_OPERATOR_MULTIPLICATION, // *
    TOKEN_OPERATOR_DIVISION, // /
    TOKEN_OPERATOR_I_DIVISION,// internal token - division by integer
    // Comparison operatos
    TOKEN_OPERATOR_LESS_THAN, // <
    TOKEN_OPERATOR_GREATER_THAN, // >
    TOKEN_OPERATOR_LESS_THAN_OR_EQUAL, // <=
    TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL, // >=
    TOKEN_OPERATOR_EQUALS, // ==
    TOKEN_OPERATOR_NOT_EQUALS, // !=
    // Literals
    TOKEN_LITERAL_INT, // an int literal
    TOKEN_LITERAL_DOUBLE, // a double literal
    TOKEN_LITERAL_STRING, // a string literal
    TOKEN_LITERAL_NIL,    // a nil literal
    //TOKEN_LITERAL_MULTILINE_STRING, // a multiline string literal // this can be passed as a regular string
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

    //Precedent table tokens - not actual tokens from source file so dont worry about them ;-)  --Jimbo
    PRECEDENT_E,
    PRECEDENT_END,
} Token;


extern Token CURRENT_TOKEN;


#define GET_TOKEN(skip_eol) \
	CURRENT_TOKEN = get_token(buffer_string, skip_eol); \
	if (CURRENT_TOKEN == TOKEN_ERR_INTERNAL || CURRENT_TOKEN == TOKEN_ERR_LEXICAL) \
		return (CURRENT_TOKEN == TOKEN_ERR_INTERNAL ? ERR_INTERNAL : ERR_LEXICAL);


#ifndef NDEBUG
static inline void print_token_as_string(Token t){
    char* tokens_as_string[] = {
        "TOKEN_ERR_LEXICAL",
        "TOKEN_ERR_INTERNAL",
        "TOKEN_EOL",
        "TOKEN_EOF",
        "TOKEN_KEYWORD_NIL",
        "TOKEN_KEYWORD_INT",
        "TOKEN_KEYWORD_DOUBLE",
        "TOKEN_KEYWORD_STRING",
        "TOKEN_KEYWORD_VAR",
        "TOKEN_KEYWORD_LET",
        "TOKEN_KEYWORD_IF",
        "TOKEN_KEYWORD_ELSE",
        "TOKEN_KEYWORD_WHILE",
        "TOKEN_KEYWORD_FUNC",
        "TOKEN_KEYWORD_RETURN",
        "TOKEN_PARENTHESIS_LEFT",
        "TOKEN_PARENTHESIS_RIGHT",
        "TOKEN_BRACE_LEFT",
        "TOKEN_BRACE_RIGHT",
        "TOKEN_OPERATOR_PLUS",
        "TOKEN_OPERATOR_CONCATENATE",
        "TOKEN_OPERATOR_MINUS",
        "TOKEN_OPERATOR_MULTIPLICATION",
        "TOKEN_OPERATOR_DIVISION",
        "TOKEN_OPERATOR_I_DIVISION",
        "TOKEN_OPERATOR_LESS_THAN",
        "TOKEN_OPERATOR_GREATER_THAN",
        "TOKEN_OPERATOR_LESS_THAN_OR_EQUAL",
        "TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL",
        "TOKEN_OPERATOR_EQUALS",
        "TOKEN_OPERATOR_NOT_EQUALS",
        "TOKEN_LITERAL_INT",
        "TOKEN_LITERAL_DOUBLE",
        "TOKEN_LITERAL_STRING",
        "TOKEN_LITERAL_NIL",
        "TOKEN_ASSIGN",
        "TOKEN_EXCLAMATION",
        "TOKEN_QUESTION",
        "TOKEN_NIL_COALESCING",
        "TOKEN_COMMA",
        "TOKEN_COLON",
        "TOKEN_ARROW",
        "TOKEN_IDENTIFIER",
        "PRECEDENT_E",
        "PRECEDENT_END",
    };
    //printf("%s\n", tokens_as_string[t]);
    printf("%s", tokens_as_string[t]);
}
#else
#pragma GCC diagnostic ignored "-Wunused-parameter"
static inline void print_token_as_string(Token t){}
#endif


typedef enum {
    // Start state
    LEXER_STATE_START, // starting state
    LEXER_STATE_POSSIBLE_COMMENT, // Leads from '/' to -> '//' '*' 
    // Identifier, possible keyword
    LEXER_STATE_IDENTIFIER_OR_KEYWORD, // State from 'LEXER_STATE_START' -> 'LEXER_STATE_INDETIFIER_OR_KEYWORD'
    // Literals
    LEXER_STATE_NUMBER,
    LEXER_STATE_DOUBLE_DOT,
    LEXER_STATE_DOUBLE_AFTER_DOT,
    LEXER_STATE_DOUBLE_E,
    LEXER_STATE_DOUBLE_E_SIGN,
    LEXER_STATE_DOUBLE_AFTER_E,
    // String literals...
    LEXER_STATE_STRING_BEGIN,
    // Normal string
    LEXER_STATE_STRING,
    LEXER_STATE_STRING_ESCAPE,
    // Multiline string
    LEXER_STATE_POSSIBLE_MULTILINE_STRING,
    LEXER_STATE_MULTILINE_STRING_OPENING,
    LEXER_STATE_MULTILNE_STRING_OPENING_EOL,
    LEXER_STATE_MULTILINE_STRING,
    LEXER_STATE_MULTILINE_STRING_ESCAPE,
    LEXER_STATE_MULTILINE_STRING_EOL,
    LEXER_STATE_POSSIBLE_MULTILINE_STRING_END,
    LEXER_STATE_MULTILINE_STRING_END,
    LEXER_STATE_MULTILINE_STRING_FIRST_QUOTE,
    LEXER_STATE_MULTILINE_STRING_SECOND_QUOTE,
    //States to check double char operators and such
    //All start from 'LEXER_STATE_START'
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


void initLexer(FILE* file);

Error save_current_file_offset();

Error rollback_to_saved_file_offset();

Token get_token(BufferString* buffer_string, bool skip_eol);

void unget_token();

Token get_next_token(BufferString* buffer_string);

#endif
