#include "syntax_ll_grammar.h"

// #include "lexer.h"



// delete after merging with lexer
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

#include "buffer_string.h"

// delete after merging with lexer
Token get_next_token(BufferString* b){
	return TOKEN_ERR;
}



bool validate_condition(Token current_token, BufferString* buffer_string){
	// check if we begin with (
	if (current_token != TOKEN_PARENTHESIS_LEFT){
		return false;
	}
	// check if the condition is empty
	current_token = get_next_token(buffer_string);
	if (current_token == TOKEN_PARENTHESIS_RIGHT){
		return false;
	}

	return true;
}


bool check_expression(Token current_token, BufferString* buffer_string, bool is_inside_parentheses){
	return true;
}