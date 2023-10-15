#include "syntax.h"

#include <stdbool.h>
#include "syntax_ll_grammar.h"
#include "buffer_string.h"



Token S_LAST_TOKEN;
Token S_CURRENT_TOKEN;
Token S_LAST_TOKEN;


BufferString b;




Error parse(SymTable* sym_table){
	return ll_program(&b);
}


// void free_resources(SymTable* symtable, BufferString* buffer_string){

// }



// Error parse(SymTable* sym_table){
// 	// init BufferString
// 	BufferString buffer_string;
// 	if(!buffer_string_init(&buffer_string)){
// 		return ERR_INTERNAL;
// 	}

// 	Token current_token = get_next_token(&buffer_string);
// 	while (current_token != TOKEN_EOF){
// 		current_token = get_next_token(&buffer_string);
// 		switch (current_token){
// 			case TOKEN_ERR:
// 				return ERR_LEXICAL;

// 			case TOKEN_EOL:
// 				break;
			
// 			case TOKEN_KEYWORD_NIL:
// 				break;
			
// 			case TOKEN_KEYWORD_INT:
// 				break;
			
// 			case TOKEN_KEYWORD_DOUBLE:
// 				break;
			
// 			case TOKEN_KEYWORD_STRING:
// 				break;
			
// 			case TOKEN_KEYWORD_VAR:
// 				break;
			
// 			case TOKEN_KEYWORD_LET:
// 				break;
			
// 			case TOKEN_KEYWORD_IF:
// 				validate_condition(get_next_token(&b), &b);
// 				break;
			
// 			case TOKEN_KEYWORD_ELSE:
// 				break;
			
// 			case TOKEN_KEYWORD_WHILE:
// 				break;
			
// 			case TOKEN_KEYWORD_FUNC:
// 				break;
			
// 			case TOKEN_KEYWORD_RETURN:
// 				break;
			


			
// 			default:
// #ifndef NDEBUG
// 				printf("Non Token enum value was returned or handling for this Token is not implemented. Token value: %d\n", current_token);
// 				assert(false);
// #endif
// 				return ERR_DEBUG;
// 		}
// 	}
// 	// Now we are outside the while loop, so EOF must have been reached




// 	return ERR_OK;
// }
