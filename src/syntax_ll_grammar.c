#include "syntax_ll_grammar.h"


#include <stdio.h>


int x = 0;
// delete after merging with lexer
Token get_next_token(BufferString* b){
	return TOKEN_ERR + x++;
}

void unget_token(){

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
	// TOKEN_KEYWORD_FUNC
	if (get_next_token(buffer_string) != TOKEN_KEYWORD_FUNC)
		return ERR_SYNTAX;
	// TOKEN_IDENTIFIER
	if (get_next_token(buffer_string) != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	// TOKEN_PARENTHESIS_LEFT
	if (get_next_token(buffer_string) != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	// <func_params>
	Error err = ll_func_params(buffer_string);
	if (err)
		return err;
	// TOKEN_PARENTHESIS_RIGHT
	if (get_next_token(buffer_string) != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;
	// <func_type>
	err = ll_func_type(buffer_string);
	if (err)
		return err;

	return OK;
}


// <func_body>
Error ll_func_body(BufferString* buffer_string){
	if (get_next_token(buffer_string) != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;
	Error err = ll_statements(buffer_string);
	if (err)
		return err;
	if (get_next_token(buffer_string) != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;
	return OK;
}


// <func_params>
Error ll_func_params(BufferString* buffer_string){
	// ɛ
	if (get_next_token(buffer_string) != TOKEN_IDENTIFIER){
		unget_token();
		return OK;
	}
	// <func_param>
	Error err = ll_func_param(buffer_string);
	if (err)
		return err;
	// <func_more_params>
	return ll_func_more_params(buffer_string);
}


// <func_param>
Error ll_func_param(BufferString* buffer_string){
	// TOKEN_IDENTIFIER - param name
	if (get_next_token(buffer_string) != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	// TOKEN_IDENTIFIER - param identifier
	if (get_next_token(buffer_string) != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	// TOKEN_COLON
	if (get_next_token(buffer_string) != TOKEN_COLON)
		return ERR_SYNTAX;
	// <type>
	return ll_type(buffer_string);
}


Error ll_func_more_params(BufferString* buffer_string){
	// ɛ
	if (get_next_token(buffer_string) != TOKEN_COMMA){
		unget_token();
		return OK;
	}
	// TOKEN_COMMA
	// <func_param>
	Error err = ll_func_param(buffer_string);
	if (err)
		return err;
	// <func_more_params>
	return ll_func_more_params(buffer_string);
}



Error ll_func_type(BufferString* buffer_string){
	if (get_next_token(buffer_string) != TOKEN_ARROW){
		unget_token();
		return OK;
	}
	return ll_type(buffer_string);
}


Error ll_type(BufferString* buffer_string){
	switch (get_next_token(buffer_string)){
		case TOKEN_KEYWORD_INT:
		case TOKEN_KEYWORD_DOUBLE:
		case TOKEN_KEYWORD_STRING:
			return OK;
		default:
			return ERR_SYNTAX;
	}
}


// <statements>
Error ll_statements(BufferString* buffer_string){
	return OK;
}


// <program>
Error ll_program(BufferString* buffer_string){
	Error err;
	// diky za najiti chyby Jimbo <3
	Token current_token = get_next_token(buffer_string);
	// <program> -> TOKEN_EOL <program>
	if (current_token == TOKEN_EOL)
		return ll_program(buffer_string);

	// <program> -> TOKEN_EOF
	if (current_token == TOKEN_EOF)
		return OK;
	
	// <program> -> <func_definitions>
	if (current_token == TOKEN_KEYWORD_FUNC){
		unget_token();
		err = ll_func_definition(buffer_string);
		return err ? err : ll_program(buffer_string);
	}

	// <program> -> <statements>
	err = ll_statements(buffer_string);
	return err ? err : ll_program(buffer_string);
}