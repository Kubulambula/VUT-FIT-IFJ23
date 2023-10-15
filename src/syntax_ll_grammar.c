#include "syntax_ll_grammar.h"


#include <stdio.h>


int x = 0;
// delete after merging with lexer
Token get_next_token(BufferString* b){
	return TOKEN_ERR + x++;
}




// static Token LAST_TOKEN = TOKEN_ERR;
// static Token CURRENT_TOKEN = TOKEN_ERR;
// #define NEXT_TOKEN _GET_NEXT_TOKEN(buffer_string)


// static inline Token _GET_NEXT_TOKEN(BufferString* buffer_string){
// 	LAST_TOKEN = CURRENT_TOKEN;
// 	CURRENT_TOKEN = get_next_token(buffer_string);
// 	return CURRENT_TOKEN;
// }

Error ll_program(BufferString* buffer_string){
	return ERR_OK;
}