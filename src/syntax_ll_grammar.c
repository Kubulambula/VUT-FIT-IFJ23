#include "syntax_ll_grammar.h"
#include "lexer.h"
#include "lexer.c"
#include <stdio.h>

Token current_token;

void unget_token(){

}

Error ll_while(BufferString* buffer_string){
	Error err = ll_while_head(buffer_string);
	if (err)
		return err;
	return ll_statement_body(buffer_string);
}


//  TOKEN_KEYWORD_WHILE TOKEN_PARENTHESIS_LEFT <expression> TOKEN_PARENTHESIS_RIGHT
Error ll_while_head(BufferString* buffer_string){
	// TOKEN_KEYWORD_WHILE
	if (get_next_token(buffer_string) != TOKEN_KEYWORD_WHILE)
		return ERR_SYNTAX;
	// TOKEN_PARENTHESIS_LEFT
	if (get_next_token(buffer_string) != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	// <func_params>
	Error err = ll_expressions(buffer_string);
	if (err)
		return err;
	// TOKEN_PARENTHESIS_RIGHT
	if (get_next_token(buffer_string) != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;
	
	return OK;
}


Error ll_if(BufferString* buffer_string){
	// if_head
	Error err = ll_if_head(buffer_string);
	if (err)
		return err;
	// <statement_body> - if body
	err = ll_statement_body(buffer_string);
	if (err)
		return err;
	// TOKEN_KEYWORD_ELSE
	if (get_next_token(buffer_string) != TOKEN_KEYWORD_ELSE)
		return ERR_SYNTAX;
	// <statement_body> - else body
	err = ll_statement_body(buffer_string);
	return err;
}


// TOKEN_KEYWORD_IF TOKEN_PARENTHESIS_LEFT <expression> TOKEN_PARENTHESIS_RIGHT
// TOKEN_KEYWORD_IF TOKEN_KEYWORD_LET TOKEN_IDENTIFIER
Error ll_if_head(BufferString* buffer_string){
	// TOKEN_KEYWORD_IF
	Token current_token = get_next_token(buffer_string);
	if (current_token != TOKEN_KEYWORD_IF)
		return ERR_SYNTAX;
	// TOKEN_PARENTHESIS_LEFT | TOKEN_KEYWORD_LET
	current_token = get_next_token(buffer_string);
	// TOKEN_PARENTHESIS_LEFT
	if (current_token == TOKEN_PARENTHESIS_LEFT){
		// <expression>
		Error err = ll_expressions(buffer_string);
		if (err)
			return err;
		// TOKEN_PARENTHESIS_RIGHT
		return (get_next_token(buffer_string) == TOKEN_PARENTHESIS_RIGHT) ? OK : ERR_SYNTAX;

	// TOKEN_KEYWORD_LET
	} else if (current_token == TOKEN_KEYWORD_LET){
		return (get_next_token(buffer_string) == TOKEN_IDENTIFIER) ? OK : ERR_SYNTAX;
	}
	return ERR_SYNTAX;
}


// <func_definitnion>
Error ll_func_definition(BufferString* buffer_string){
	Error err = ll_func_head(buffer_string);
	if (err)
		return err;
	return ll_statement_body(buffer_string);
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


Error ll_literal(BufferString* buffer_string){
	switch (get_next_token(buffer_string)){
		case TOKEN_LITEREAL_INT:
		case TOKEN_LITEREAL_DOUBLE:
		case TOKEN_LITEREAL_STRING:
			return OK;
		default:
			return ERR_SYNTAX;
	}
}


Error ll_statement_body(BufferString* buffer_string){
	if (get_next_token(buffer_string) != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;
	Error err = ll_statements(buffer_string);
	if (err)
		return err;
	if (get_next_token(buffer_string) != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;
	return OK;
}


// <statements>
// TODO
Error ll_statements(BufferString* buffer_string){
	Token current_token = get_next_token(buffer_string);
	switch (current_token){
		case TOKEN_KEYWORD_IF:
			/* code */
			break;
		case TOKEN_KEYWORD_WHILE:
			break;
		
		default:
			break;
	}
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

//<lit>
bool ll_lit(BufferString* buffer_string){
	printf("ll_lit\n");
	switch(current_token){
		case TOKEN_LITEREAL_INT:	//<lit> -> #Int literal
		case TOKEN_LITEREAL_DOUBLE:	//<lit> -> #Double literal
		case TOKEN_LITEREAL_STRING:	//<lit> -> #String literal
			current_token = get_next_token(buffer_string);
			printf("true\n");
			printf(")\n");
			return true;
			break;
		default:
			printf("false\n");
			printf(")\n");
			return false;
			break;
	}
}

// <val>
bool ll_val(BufferString* buffer_string){
	printf("ll_val(\n");
	switch(current_token){
		case TOKEN_IDENTIFIER:			//<val> -> #identif
			current_token = get_next_token(buffer_string);
			printf("true\n");
			printf(")\n");
			return true;	
			break;
		case TOKEN_PARENTHESIS_LEFT:	//<val> -> ( <expressions> )
			current_token = get_next_token(buffer_string);
			return ll_expressions(buffer_string) && (current_token == TOKEN_PARENTHESIS_RIGHT);
			break;
		default:						//<val> -> <lit>
			return ll_lit(buffer_string);
			break;
	}
}

// <more_val>
bool ll_more_val(BufferString* buffer_string){
	printf("ll_more_val(\n");
	switch(current_token){
		case TOKEN_OPERATOR_PLUS:			//<more_val> -> + <val><more_val>
		case TOKEN_OPERATOR_MINUS:			//<more_val> -> - <val><more_val>
		case TOKEN_OPERATOR_MULTIPLICATION:	//<more_val> -> * <val><more_val>
		case TOKEN_OPERATOR_DIVISION:		//<more_val> -> / <val><more_val>
			current_token = get_next_token(buffer_string);
			return ll_val(buffer_string) && ll_more_val(buffer_string);
			break;
		//<more_val> -> ɛ :
		case TOKEN_EOF:
		case TOKEN_EOL:
		case TOKEN_BRACE_RIGHT:						// }
		case TOKEN_PARENTHESIS_RIGHT:				// )
		case TOKEN_OPERATOR_LESS_THAN:				// <
    	case TOKEN_OPERATOR_GREATER_THAN:			// >
    	case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:		// <=
    	case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL:	// >=
    	case TOKEN_OPERATOR_EQUALS:					// ==
    	case TOKEN_OPERATOR_NOT_EQUALS:				// !=
			printf("true\n");
			printf(")\n");
			return true;
			break;
		default:
			printf("false\n");
			printf(")\n");
			return false;
	}
}

// <exp>
bool ll_exp(BufferString* buffer_string){
	printf("ll_exp: ");
	switch(current_token){
		case TOKEN_EXCLAMATION:	// <exp> -> !<exp>
			current_token = get_next_token(buffer_string);
			return ll_exp(buffer_string) && ll_more_exp(buffer_string);
			break;
		default:				//<exp> -> <val><more_val>
			return ll_val(buffer_string) && ll_more_val(buffer_string);
	}
}

// <more_exp>
bool ll_more_exp(BufferString* buffer_string){
	printf("ll_more_exp(\n");
	switch(current_token){
		case TOKEN_OPERATOR_LESS_THAN:				// <more_exp> -> < <exp><more_exp>
    	case TOKEN_OPERATOR_GREATER_THAN:			// <more_exp> -> > <exp><more_exp>
    	case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:		// <more_exp> -> <= <exp><more_exp>
    	case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL:	// <more_exp> -> >= <exp><more_exp>
    	case TOKEN_OPERATOR_EQUALS:					// <more_exp> -> == <exp><more_exp>
    	case TOKEN_OPERATOR_NOT_EQUALS:				// <more_exp> -> != <exp><more_exp>
			current_token = get_next_token(buffer_string);
			return ll_exp(buffer_string) && ll_more_exp(buffer_string); 
			break;
		// <more_exp> -> ɛ :
		case TOKEN_EOF:								// EOF
		case TOKEN_EOL:								// EOL
		case TOKEN_BRACE_RIGHT:						// }
		case TOKEN_PARENTHESIS_RIGHT:				// )	<= pro jistotu
		printf("true\n");
		printf(")\n");
			return true;
			break;
		default:
		printf("false\n");
		printf(")\n");
			return false;
	}
}

// <expressions>
bool ll_expressions(BufferString* buffer_string){
	printf("ll_expressions(\n");
	return ll_exp(buffer_string) && ll_more_exp(buffer_string); //<expressions> -> <exp><more_exp>
}