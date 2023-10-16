#include "syntax_ll_grammar.h"


#include <stdio.h>


int x = 0;
// delete after merging with lexer
Token get_next_token(BufferString* b){
	return TOKEN_ERR + x++;
}


// <func_definitnion>
Error ll_func_definition(BufferString* buffer_string){
	Error err = ll_func_head(buffer_string);
	if (err)
		return err;
	return ll_func_body(buffer_string);
}


// <func_head>
// TOKEN_KEYWORD_FUNC TOKEN_IDENTIFIER TOKEN_PARENTHESIS_LEFT <func_params> TOKEN_PARENTHESIS_RIGHT <func_type>
Error ll_func_head(BufferString* buffer_string){
	// TOKEN_IDENTIFIER
	if (get_next_token(buffer_string) != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	// TOKEN_PARENTHESIS_LEFT
	if (get_next_token(buffer_string) != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	// <func_params>
	Error err = ll_func_params(buffer_string);

	return err;
}


// <func_body>
Error ll_func_body(BufferString* buffer_string){
	return OK;
}


// <func_params>
Error ll_func_params(BufferString* buffer_string){
	return OK;
}


// <statements>
Error ll_statements(BufferString* buffer_string){
	return OK;
}


// <program>
Error ll_program(BufferString* buffer_string){
	Error err;
	// <program> -> TOKEN_EOL <program>
	if (get_next_token(buffer_string) == TOKEN_EOL)
		return ll_program(buffer_string);

	// <program> -> TOKEN_EOF
	if (get_next_token(buffer_string) == TOKEN_EOF)
		return OK;
	
	// <program> -> <func_definitions>
	if (get_next_token(buffer_string) == TOKEN_KEYWORD_FUNC){
		err = ll_func_definition(buffer_string);
		return err ? err : ll_program(buffer_string);
	}

	// <program> -> <statements>
	err = ll_statements(buffer_string);
	return err ? err : ll_program(buffer_string);
}