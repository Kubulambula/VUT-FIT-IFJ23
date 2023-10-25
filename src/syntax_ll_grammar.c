#include "syntax_ll_grammar.h"
#include "lexer.h"
//#include "lexer.c"
#include <stdio.h>

Token current_token;

void unget_token(){

}


Error ll_while(BufferString* buffer_string){
	printf("ll_while\n");
	printf("Token: %d\n", current_token);
	Error err = ll_while_head(buffer_string);
	if (err)
		return err;
		printf("ERR ll_while\n");
		printf("Token: %d\n", current_token);
	return ll_statement_body(buffer_string);
}


//  TOKEN_KEYWORD_WHILE TOKEN_PARENTHESIS_LEFT <expression> TOKEN_PARENTHESIS_RIGHT
Error ll_while_head(BufferString* buffer_string){
	printf("ll_while_head\n");
	printf("Token: %d\n", current_token);
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
	printf("ll_if\n");
	current_token = get_next_token(buffer_string);
	printf("Token: %d\n", current_token);

	if(ll_if_head(buffer_string))
		return ERR_SYNTAX;

	if(ll_statement_body(buffer_string))
		return ERR_SYNTAX;

	current_token = get_next_token(buffer_string);
	if (current_token != TOKEN_KEYWORD_ELSE)
		return ERR_SYNTAX;

	if(ll_statement_body(buffer_string))
		return ERR_SYNTAX;

	return OK;
}


Error ll_if_head(BufferString* buffer_string){
	printf("ll_if_head\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_KEYWORD_LET:
		case TOKEN_KEYWORD_VAR:
			current_token = get_next_token(buffer_string);
			printf("check %d\n", current_token);
			if(current_token != TOKEN_IDENTIFIER)
				return ERR_SYNTAX;

			current_token = get_next_token(buffer_string);
		default:
			return ll_expressions(buffer_string);
	}
}


// <func_definitnion>
Error ll_func_definition(BufferString* buffer_string){
	printf("ll_func_definition\n");
	printf("Token: %d\n", current_token);
	Error err = ll_func_head(buffer_string);
	if (err)
		return err;
	return ll_statement_body(buffer_string);
}


// <func_head>
// TOKEN_KEYWORD_FUNC TOKEN_IDENTIFIER TOKEN_PARENTHESIS_LEFT <func_params> TOKEN_PARENTHESIS_RIGHT <func_type>
Error ll_func_head(BufferString* buffer_string){
	printf("ll_func_head\n");
	printf("Token: %d\n", current_token);
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
	printf("ll_func_params\n");
	printf("Token: %d\n", current_token);
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
	printf("ll_func_param\n");
	printf("Token: %d\n", current_token);
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
	printf("ll_more_params\n");
	printf("Token: %d\n", current_token);
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
	printf("ll_func_type\n");
	printf("Token: %d\n", current_token);
	if (get_next_token(buffer_string) != TOKEN_ARROW){
		unget_token();
		return OK;
	}
	return ll_type(buffer_string);
}

Error ll_statement_body(BufferString* buffer_string){
	printf("ll_statement_body\n");
	printf("Token: %d\n", current_token);
	if (current_token != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;

	current_token = get_next_token(buffer_string);
	if(ll_statements(buffer_string))
		return ERR_SYNTAX;

	if(current_token != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;

	return OK;
}


// <program>
Error ll_program(BufferString* buffer_string){
	printf("ll_program\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_EOF:		//<program> -> ɛ
			return OK;

		case TOKEN_EOL:		//<program> -> EOL<program>
			current_token = get_next_token(buffer_string);
			return	ll_program(buffer_string);

		case TOKEN_KEYWORD_FUNC:	//<program> -> <func_def>
			current_token = get_next_token(buffer_string);
			if(ll_func_definition(buffer_string)){
				printf("ERR ll_program\n");
				printf("Token: %d\n", current_token);
				return ERR_SYNTAX;}
			current_token = get_next_token(buffer_string);
			return ll_program(buffer_string);

		default:					//<program> -> <statements>
			if(ll_statements(buffer_string))
				return ERR_SYNTAX;
			return ll_program(buffer_string);
	}
}

Error ll_func_call(BufferString* buffer_string){
	if(current_token != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;

	current_token = get_next_token(buffer_string);
	if(ll_func_args(buffer_string))
		return ERR_SYNTAX;

	if(current_token != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;

	return OK;
}

Error ll_func_args(BufferString* buffer_string){
	if(ll_func_arg(buffer_string))
		return ERR_SYNTAX;

	return ll_func_more_arg(buffer_string);
}

Error ll_func_arg(BufferString* buffer_string){
	switch(current_token){
		case TOKEN_PARENTHESIS_RIGHT:
			return OK;

		default:
			return ll_expressions(buffer_string);
	}
}

Error ll_func_more_arg(BufferString* buffer_string){
	switch(current_token){
		case TOKEN_COMMA:
			current_token = get_next_token(buffer_string);
			if(ll_func_arg(buffer_string))
				return ERR_SYNTAX;
			return ll_func_more_arg(buffer_string);

		case TOKEN_PARENTHESIS_RIGHT:
			return OK;

		default:
			return ERR_SYNTAX;
	}
}


Error ll_type(BufferString* buffer_string){
	printf("ll_type\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_LITERAL_INT:
		case TOKEN_LITERAL_DOUBLE:
		case TOKEN_LITERAL_STRING:
			current_token = get_next_token(buffer_string);
			if(current_token = TOKEN_QUESTION){
				current_token = get_next_token(buffer_string);
			}
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

//<lit>
Error ll_lit(BufferString* buffer_string){
	printf("ll_lit\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_LITERAL_INT:	//<lit> -> #Int literal
		case TOKEN_LITERAL_DOUBLE:	//<lit> -> #Double literal
		case TOKEN_LITERAL_STRING:	//<lit> -> #String literal
			current_token = get_next_token(buffer_string);
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

//<identifier>
Error ll_identifier(BufferString* buffer_string){
	printf("ll_identifier\n");
	current_token = get_next_token(buffer_string);
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_PARENTHESIS_LEFT:	//<idnetifier> -> #identifier<func_call>
			if(ll_func_args(buffer_string))
				return ERR_SYNTAX;
			
			if(current_token != TOKEN_PARENTHESIS_RIGHT)
				return ERR_SYNTAX;

			return OK;

		default:
			return ll_more_exp(buffer_string);
	}
}

// <val>
Error ll_val(BufferString* buffer_string){
	printf("ll_val\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_IDENTIFIER:			//<val> -> <identifier>
			return ll_identifier(buffer_string);	

		case TOKEN_PARENTHESIS_LEFT:	//<val> -> ( <expressions> )
			current_token = get_next_token(buffer_string);
			printf("Token: %d\n", current_token);
			if(ll_expressions(buffer_string))
				return ERR_SYNTAX;

			if(current_token != TOKEN_PARENTHESIS_RIGHT)
				return ERR_SYNTAX;
			return OK;

		default:						//<val> -> <lit>
			return ll_lit(buffer_string);
	}
}

// <more_val>
Error ll_more_val(BufferString* buffer_string){
	printf("ll_more_val\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_OPERATOR_PLUS:			//<more_val> -> + <val><more_val>
		case TOKEN_OPERATOR_MINUS:			//<more_val> -> - <val><more_val>
		case TOKEN_OPERATOR_MULTIPLICATION:	//<more_val> -> * <val><more_val>
		case TOKEN_OPERATOR_DIVISION:		//<more_val> -> / <val><more_val>
			current_token = get_next_token(buffer_string);
			printf("Token: %d\n", current_token);
			if(ll_val(buffer_string))
				return ERR_SYNTAX;

			return ll_more_val(buffer_string);

		//<more_val> -> ɛ :
		case TOKEN_EOF:
		case TOKEN_EOL:
		case TOKEN_BRACE_RIGHT:						// }
		case TOKEN_BRACE_LEFT:						// {
		case TOKEN_PARENTHESIS_RIGHT:				// )
		case TOKEN_OPERATOR_LESS_THAN:				// <
    	case TOKEN_OPERATOR_GREATER_THAN:			// >
    	case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:		// <=
    	case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL:	// >=
    	case TOKEN_OPERATOR_EQUALS:					// ==
    	case TOKEN_OPERATOR_NOT_EQUALS:				// !=
			return OK;

		default:
			printf("ERR ll_val\n");
			printf("Token: %d\n", current_token);
			return ERR_SYNTAX;
	}
}

// <exp>
Error ll_exp(BufferString* buffer_string){
	printf("ll_exp\n");
	printf("Token: %d\n", current_token);
	if(ll_val(buffer_string))
		return ERR_SYNTAX;
	return ll_more_val(buffer_string);
}

// <more_exp>
Error ll_more_exp(BufferString* buffer_string){
	printf("ll_more_exp\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_OPERATOR_LESS_THAN:				// <more_exp> -> < <exp><more_exp>
    	case TOKEN_OPERATOR_GREATER_THAN:			// <more_exp> -> > <exp><more_exp>
    	case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:		// <more_exp> -> <= <exp><more_exp>
    	case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL:	// <more_exp> -> >= <exp><more_exp>
    	case TOKEN_OPERATOR_EQUALS:					// <more_exp> -> == <exp><more_exp>
    	case TOKEN_OPERATOR_NOT_EQUALS:				// <more_exp> -> != <exp><more_exp>
		case TOKEN_NIL_COALESCING:					// <more_exp> -> ?? <exp><more_exp>
			current_token = get_next_token(buffer_string);
			printf("Token: %d\n", current_token);
			if(ll_exp(buffer_string))
				return ERR_SYNTAX;

			return ll_more_exp(buffer_string); 

		case TOKEN_EXCLAMATION:						// <more_exp> -> !<more_exp>	debilní polskej unární postfixovej operátor
			current_token = get_next_token(buffer_string);
			printf("Token: %d\n", current_token);
			return ll_more_exp(buffer_string);

		// <more_exp> -> ɛ :
		case TOKEN_EOF:								// EOF
		case TOKEN_EOL:								// EOL
		case TOKEN_BRACE_LEFT:						// {
		case TOKEN_BRACE_RIGHT:						// }
		case TOKEN_PARENTHESIS_RIGHT:				// )	<= pro jistotu
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

// <expressions>
Error ll_expressions(BufferString* buffer_string){
	printf("ll_expressions\n");
	printf("Token: %d\n", current_token);
	if(ll_exp(buffer_string))
		return ERR_SYNTAX;
	
	return ll_more_exp(buffer_string); //<expressions> -> <exp><more_exp>
}

//<assign>
Error ll_assign(BufferString* buffer_string){
	printf("ll_assign\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_ASSIGN:			//<assign> -> = <expressions>
			current_token = get_next_token(buffer_string);
			printf("Token: %d\n", current_token);
			return ll_expressions(buffer_string);

		case TOKEN_EOF:				//<assign> -> ɛ :
		case TOKEN_EOL:
		case TOKEN_PARENTHESIS_RIGHT:
		case TOKEN_BRACE_RIGHT:
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

Error ll_assign_type(BufferString* buffer_string){
	printf("ll_assign_type\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_COLON:
			current_token = get_next_token(buffer_string);
			if(ll_type(buffer_string)){
				printf("Token: %d\n", current_token);
				return ERR_SYNTAX;}
			
		default:
			return ll_assign(buffer_string);
	}
}

//<return>
Error ll_return(BufferString* buffer_string){
	printf("ll_return\n");
	current_token = get_next_token(buffer_string);
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_EOL:					//<return> -> return
		case TOKEN_EOF:
		case TOKEN_PARENTHESIS_RIGHT:	//<return> -> return
		case TOKEN_BRACE_RIGHT:
			return OK;

		default:						//<return> -> return<expressions>
			return ll_expressions(buffer_string);	
	}
}

//<statement>
Error ll_statement(BufferString* buffer_string){
	printf("ll_statement\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_KEYWORD_IF:		//<statement> -> <if>
			return ll_if(buffer_string);

		case TOKEN_KEYWORD_WHILE:	//<statement> -> <while>
			current_token = get_next_token(buffer_string);
			return ll_while(buffer_string);

		case TOKEN_KEYWORD_RETURN:	//<statement> -> <return>
			return ll_return(buffer_string);

		case TOKEN_IDENTIFIER:		//<statement> -> <identifier>
			return ll_identifier(buffer_string);

		case TOKEN_KEYWORD_LET:		//<statement> -> let #identifier <assign_type> <assign>
		case TOKEN_KEYWORD_VAR:		//<statement> -> let #identifier <assign_type> <assign>
		//case TOKEN_KEYWORD_NIL:		//not sure about that
			current_token = get_next_token(buffer_string);
			printf("Token: %d\n", current_token);
			if(current_token != TOKEN_IDENTIFIER)
				return ERR_SYNTAX;

			current_token = get_next_token(buffer_string);
			return ll_assign_type(buffer_string);

		default:
			return ERR_SYNTAX;
	}
}

//<statements>
Error ll_statements(BufferString* buffer_string){
	Error err;
	printf("ll_statements\n");
	printf("Token: %d\n", current_token);
	switch(current_token){
		case TOKEN_BRACE_RIGHT:	//<statements> -> ɛ
		case TOKEN_EOF:
			return OK;

		case TOKEN_EOL:
			current_token = get_next_token(buffer_string);
		default:				//<statements> -> <statement><statements>
			return ll_statement(buffer_string) || ll_statements(buffer_string);
	}
}
