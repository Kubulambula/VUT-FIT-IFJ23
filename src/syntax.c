#include "syntax.h"

#include <stdbool.h>
#include "syntax_ll_grammar.h"
#include "buffer_string.h"

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

// delete after merging with lexer
Token get_next_token(BufferString* b){
	return TOKEN_ERR;
}


BufferString b;

// void free_resources(SymTable* symtable, BufferString* buffer_string){

// }



bool validate_if(){
	return true;
}



Error parse(SymTable* sym_table){
	// init BufferString
	BufferString buffer_string;
	if(!buffer_string_init(&buffer_string)){
		return ERR_INTERNAL;
	}

	Token current_token = get_next_token(&buffer_string);
	while (current_token != TOKEN_EOF){
		current_token = get_next_token(&buffer_string);
		switch (current_token){
			case TOKEN_ERR:
				return ERR_LEXICAL;

			case TOKEN_EOL:
				break;
			
			case TOKEN_KEYWORD_NIL:
				break;
			
			case TOKEN_KEYWORD_INT:
				break;
			
			case TOKEN_KEYWORD_DOUBLE:
				break;
			
			case TOKEN_KEYWORD_STRING:
				break;
			
			case TOKEN_KEYWORD_VAR:
				break;
			
			case TOKEN_KEYWORD_LET:
				break;
			
			case TOKEN_KEYWORD_IF:
				validate_condition(get_next_token(&b), &b);
				break;
			
			case TOKEN_KEYWORD_ELSE:
				break;
			
			case TOKEN_KEYWORD_WHILE:
				break;
			
			case TOKEN_KEYWORD_FUNC:
				break;
			
			case TOKEN_KEYWORD_RETURN:
				break;
			


			
			default:
#ifndef NDEBUG
				printf("Non Token enum value was returned or handling for this Token is not implemented. Token value: %d\n", current_token);
				assert(false);
#endif
				return ERR_DEBUG;
		}
	}
	// Now we are outside the while loop, so EOF must have been reached




	return ERR_OK;
}
