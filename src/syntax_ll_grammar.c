#include "syntax_ll_grammar.h"


#include <stdio.h>


int x = 0;
// delete after merging with lexer
Token get_next_token(BufferString* b){
	return TOKEN_ERR + x++;
}



Error ll_program(BufferString* buffer_string){
	return ERR_OK;
}