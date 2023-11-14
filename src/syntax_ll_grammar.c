#include <stdio.h>

#include "syntax_ll_grammar.h"




Error ll_while(BufferString* buffer_string){
	Error err = ll_while_head(buffer_string);
	if (err)
		return err;
	return ll_statement_body(buffer_string);
}


Error ll_let_var_definitnion(BufferString* buffer_string){
	Token current_token = get_next_token(buffer_string);
	if (current_token == TOKEN_KEYWORD_VAR){
		return ll_var_definition(buffer_string);
	} else if (current_token == TOKEN_KEYWORD_LET){
		// TODO - replace with ll_let_definition once completed (after completing the AST stuff)
		return ll_var_definition(buffer_string);
	}
	return ERR_SYNTAX;
}


Error ll_var_definition(BufferString* buffer_string){
	Error err;
	Token current_token = get_next_token(buffer_string);
	// TOKEN_IDENTIFIER
	if ((current_token = get_next_token(buffer_string)) != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	// TOKEN_COLON
	if ((current_token = get_next_token(buffer_string)) != TOKEN_COLON)
		return ERR_SYNTAX;
	// TOKEN_ASSIGN
	if((current_token = get_next_token(buffer_string) == TOKEN_ASSIGN)){
		err = ll_expression(buffer_string);
		if (err)
			return err;
		return OK;
	}
	// <type>
	unget_token();
	err = ll_type(buffer_string);
	if (err)
		return err;
	// TOKEN_ASSIGN
	if((current_token = get_next_token(buffer_string) != TOKEN_ASSIGN)){
		unget_token();
		return OK;
	}
	err = ll_expression(buffer_string);
	return err;
	return ERR_SYNTAX;
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
	Error err = ll_expression(buffer_string);
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
		Error err = ll_expression(buffer_string);
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
		case TOKEN_LITERAL_INT:
		case TOKEN_LITERAL_DOUBLE:
		case TOKEN_LITERAL_STRING:
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

Error ll_expression(BufferString* buffer_string){
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
Error ll_lit(BufferString* buffer_string, Token t){
	switch(t){
		case TOKEN_LITERAL_INT:	//<lit> -> #Int literal
		case TOKEN_LITERAL_DOUBLE:	//<lit> -> #Double literal
		case TOKEN_LITERAL_STRING:	//<lit> -> #String literal
			return true;
			break;
		default:
			return false;
			break;
	}
}

// <val>
Error ll_val(BufferString* buffer_string, Token t){
	switch(t){
		case TOKEN_IDENTIFIER:			//<val> -> #identif
			return true;	
			break;
		// case TOKEN_KEYWORD_FUNC:	 	//<val> -> <func_call>
		// 	return true; // doplnit az bude funkce napsana
		// 	break;
		case TOKEN_PARENTHESIS_LEFT:	//<val> -> (<expressions>)
											  // first token of <expressions>		right parenthesis
			return ll_expressions(buffer_string, get_next_token(buffer_string)) && (get_next_token(buffer_string) == TOKEN_PARENTHESIS_RIGHT);
			break;
		default:
			return ll_lit(buffer_string, t);
			break;
	}
}

// <more_val>
Error ll_more_val(BufferString* buffer_string, Token t){
	switch(t){
		case TOKEN_OPERATOR_PLUS:			//<more_val> -> + <val><more_val>
		case TOKEN_OPERATOR_MINUS:			//<more_val> -> - <val><more_val>
		case TOKEN_OPERATOR_MULTIPLICATION:	//<more_val> -> * <val><more_val>
		case TOKEN_OPERATOR_DIVISION:		//<more_val> -> / <val><more_val>
									  // first token of <val>										  first token of <more_val>
			return ll_val(buffer_string, get_next_token(buffer_string)) && ll_more_val(buffer_string, get_next_token(buffer_string));
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
			return true;
			break;
		default:
			return false;
	}
}

// <exp>
Error ll_exp(BufferString* buffer_string, Token t){
	switch(t){
		case TOKEN_EXCLAMATION:	// <exp> -> !<exp>
			return ll_exp(buffer_string, get_next_token(buffer_string)) && ll_more_exp(buffer_string, get_next_token(buffer_string));
			break;
		default:
			return ll_val(buffer_string, get_next_token(buffer_string)) && ll_more_val(buffer_string, get_next_token(buffer_string));
	}
}

// <more_exp>
Error ll_more_exp(BufferString* buffer_string, Token t){
	switch(t){
		case TOKEN_OPERATOR_LESS_THAN:				// <more_exp> -> < <exp><more_exp>
    	case TOKEN_OPERATOR_GREATER_THAN:			// <more_exp> -> > <exp><more_exp>
    	case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:		// <more_exp> -> <= <exp><more_exp>
    	case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL:	// <more_exp> -> >= <exp><more_exp>
    	case TOKEN_OPERATOR_EQUALS:					// <more_exp> -> == <exp><more_exp>
    	case TOKEN_OPERATOR_NOT_EQUALS:				// <more_exp> -> != <exp><more_exp>
			return ll_exp(buffer_string, get_next_token(buffer_string)) && ll_more_exp(buffer_string, get_next_token(buffer_string)); 
			break;
		// <more_exp> -> ɛ :
		case TOKEN_EOF:								// EOF
		case TOKEN_EOL:								// EOL
		case TOKEN_BRACE_RIGHT:						// }
		case TOKEN_PARENTHESIS_RIGHT:				// )	<= pro jistotu
			return true;
			break;
		default:
			return false;
	}
}

// <expressions>
Error ll_expressions(BufferString* buffer_string, Token t){
	//<expressions> -> <exp><more_exp>
	return ll_exp(buffer_string, get_next_token(buffer_string)) && ll_more_exp(buffer_string, get_next_token(buffer_string));
}