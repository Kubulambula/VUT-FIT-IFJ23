#include "syntax_ll_grammar.h"
#include "lexer.h"
#include <stdio.h>

//pomocná proměná pro přehlednost pře debugování
int line = 1;

//pomocna funkce pro hledani chyb
void ll_log(const char *function_name){
	printf("%d %s : ", line, function_name);
	print_token_as_string(CURRENT_TOKEN);
}



Error ll_while(BufferString* buffer_string){
	ll_log("ll_while");
	if(ll_while_head(buffer_string))
		return ERR_SYNTAX;

	return ll_statement_body(buffer_string);
}

Error ll_while_head(BufferString* buffer_string){
	ll_log("ll_while_head");
	if(precedent(buffer_string))
		return ERR_SYNTAX;

	if(CURRENT_TOKEN == TOKEN_EOL)
		CURRENT_TOKEN = get_next_token(buffer_string);

	return OK;
}

Error ll_if(BufferString* buffer_string){
	ll_log("ll_if");
	if(ll_if_head(buffer_string))	// if <expression>
		return ERR_SYNTAX;

	if(CURRENT_TOKEN == TOKEN_EOL)	// EOL?
		CURRENT_TOKEN = get_next_token(buffer_string);

	if(ll_statement_body(buffer_string))	// {<statement_body>}
		return ERR_SYNTAX;

	if(CURRENT_TOKEN == TOKEN_EOL)	// EOL?
		CURRENT_TOKEN = get_next_token(buffer_string);

	if(CURRENT_TOKEN != TOKEN_KEYWORD_ELSE)	// else
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if(CURRENT_TOKEN == TOKEN_EOL)			// EOL?
		CURRENT_TOKEN = get_next_token(buffer_string);

	if(ll_statement_body(buffer_string))	// {<statement_body>}
		return ERR_SYNTAX;

	if(CURRENT_TOKEN == TOKEN_EOL)	// EOL?
		CURRENT_TOKEN = get_next_token(buffer_string);
	
	return OK;
}

Error ll_if_head(BufferString* buffer_string){
	ll_log("ll_if_head");
	switch(CURRENT_TOKEN){
		case TOKEN_KEYWORD_LET:
		case TOKEN_KEYWORD_VAR:
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(CURRENT_TOKEN != TOKEN_IDENTIFIER)
				return ERR_SYNTAX;

			CURRENT_TOKEN = get_next_token(buffer_string);
		default:
			return precedent(buffer_string);
	}
}

Error ll_func_definition(BufferString* buffer_string){
	ll_log("ll_func_definition");
	if(ll_func_head(buffer_string))
		return ERR_SYNTAX;

	if(CURRENT_TOKEN == TOKEN_EOL)
		CURRENT_TOKEN = get_next_token(buffer_string);
	
	return ll_statement_body(buffer_string);
}

Error ll_func_head(BufferString* buffer_string){
	ll_log("ll_func_head");
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if(ll_func_params(buffer_string))
		return ERR_SYNTAX;


	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if(ll_func_type(buffer_string))
		return ERR_SYNTAX;

	return OK;
}

Error ll_func_params(BufferString* buffer_string){
	ll_log("ll_func_params");
	if(CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT){
		CURRENT_TOKEN = get_next_token(buffer_string);
		return OK;
	}

	if(ll_func_param(buffer_string))
		return ERR_SYNTAX;

	return ll_func_more_params(buffer_string);
}

Error ll_func_param(BufferString* buffer_string){
	ll_log("ll_func_param");
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)	
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if (CURRENT_TOKEN != TOKEN_COLON)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	return ll_type(buffer_string);
}


Error ll_func_more_params(BufferString* buffer_string){
	ll_log("ll_func_more_params");
	switch(CURRENT_TOKEN){	// <func_more_params> -> ɛ
		case TOKEN_PARENTHESIS_RIGHT:
			return OK;

		case TOKEN_COMMA:	// <func_more_params> -> ,<func_param><func_more_params>
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(ll_func_param(buffer_string))
				return ERR_SYNTAX;

			return ll_func_more_params(buffer_string);
	}
}

Error ll_func_type(BufferString* buffer_string){
	ll_log("ll_func_type");
	if (CURRENT_TOKEN != TOKEN_ARROW){
		return OK;
	}
	CURRENT_TOKEN = get_next_token(buffer_string);
	return ll_type(buffer_string);
}

Error ll_statement_body(BufferString* buffer_string){
	ll_log("ll_statement_body");
	if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if(ll_statements(buffer_string))
		return ERR_SYNTAX;

	if(CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	return OK;
}

Error ll_program(BufferString* buffer_string){
	ll_log("ll_program");
	switch(CURRENT_TOKEN){
		case TOKEN_EOF:		//<program> -> ɛ
			return OK;

		case TOKEN_EOL:		//<program> -> EOL<program>
			CURRENT_TOKEN = get_next_token(buffer_string);
			return	ll_program(buffer_string);

		case TOKEN_KEYWORD_FUNC:	//<program> -> <func_def>
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(ll_func_definition(buffer_string))
				return ERR_SYNTAX;

			CURRENT_TOKEN = get_next_token(buffer_string);
			return ll_program(buffer_string);

		default:	//<program> -> <statements>
			if(ll_statements(buffer_string))
				return ERR_SYNTAX;
			return ll_program(buffer_string);
	}
}

Error ll_func_call(BufferString* buffer_string){
	ll_log("ll_func_call");
	if(CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	if(ll_func_args(buffer_string))
		return ERR_SYNTAX;

	if(CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_next_token(buffer_string);
	return OK;
}

Error ll_func_args(BufferString* buffer_string){
	ll_log("ll_func_args");
	if(CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT){
		return OK;
	}

	if(ll_func_arg(buffer_string))
		return ERR_SYNTAX;

	return ll_func_more_arg(buffer_string);
}

Error ll_func_arg(BufferString* buffer_string){
	ll_log("ll_func_arg");
	switch(CURRENT_TOKEN){
		case TOKEN_IDENTIFIER:
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(CURRENT_TOKEN == TOKEN_COLON){
				CURRENT_TOKEN = get_next_token(buffer_string);
			}
			else{
				return OK;
			}

			if(CURRENT_TOKEN == TOKEN_IDENTIFIER){
				CURRENT_TOKEN = get_next_token(buffer_string);
				return OK;
			}

		default:
			return ll_lit(buffer_string);
	}
}

Error ll_func_more_arg(BufferString* buffer_string){
	ll_log("ll_func_more_arg");
	switch(CURRENT_TOKEN){
		case TOKEN_COMMA:	// <func_more_arg> -> , <func_arg> <func_more_arg>
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(ll_func_arg(buffer_string))
				return ERR_SYNTAX;
			return ll_func_more_arg(buffer_string);

		case TOKEN_PARENTHESIS_RIGHT:	// <func_more_arg> -> ɛ
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

Error ll_type_guestion(BufferString* buffer_string){
	ll_log("ll_type_guestion");
	switch(CURRENT_TOKEN){
		case TOKEN_KEYWORD_INT:		// <type> -> Int?
		case TOKEN_KEYWORD_DOUBLE:	// <type> -> Double?
		case TOKEN_KEYWORD_STRING:	// <type> -> String?
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(CURRENT_TOKEN == TOKEN_QUESTION)
				CURRENT_TOKEN = get_next_token(buffer_string);

			return OK;

		default:
			return ERR_SYNTAX;
	}
}

Error ll_type(BufferString* buffer_string){
	ll_log("ll_type");
	switch(CURRENT_TOKEN){
		case TOKEN_KEYWORD_INT:		// <type> -> Int
		case TOKEN_KEYWORD_DOUBLE:	// <type> -> Double
		case TOKEN_KEYWORD_STRING:	// <type> -> String
			CURRENT_TOKEN = get_next_token(buffer_string);
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

Error ll_lit(BufferString* buffer_string){
	ll_log("ll_lit");
	switch(CURRENT_TOKEN){
		case TOKEN_LITERAL_INT:		//<lit> -> #Int literal
		case TOKEN_LITERAL_DOUBLE:	//<lit> -> #Double literal
		case TOKEN_LITERAL_STRING:	//<lit> -> #String literal
			CURRENT_TOKEN = get_next_token(buffer_string);
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

Error ll_assign(BufferString* buffer_string){
	ll_log("ll_assign");
	switch(CURRENT_TOKEN){
		case TOKEN_ASSIGN:	//<assign> -> = <expressions>
			CURRENT_TOKEN = get_next_token(buffer_string);
			return precedent(buffer_string);

		case TOKEN_EOF:		//<assign> -> ɛ :
		case TOKEN_EOL:
		//case TOKEN_PARENTHESIS_RIGHT:		// nevim co to tady dělá, kdyby se něco pokazilo možná je to tohle
		case TOKEN_BRACE_RIGHT:
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

Error ll_assign_type(BufferString* buffer_string){
	ll_log("ll_assign_type");
	switch(CURRENT_TOKEN){	// <assign_type> -> : <type_guestion> <assign>
		case TOKEN_COLON:
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(ll_type_guestion(buffer_string))
				return ERR_SYNTAX;
			
		default:			// <assign_type> -> <assign>
			return ll_assign(buffer_string);
	}
}

Error ll_identifier(BufferString* buffer_string){
	ll_log("ll_identifier");
	switch(CURRENT_TOKEN){
		case TOKEN_PARENTHESIS_LEFT:	// <identifier> -> #identifier <func_call>
			if(ll_func_call(buffer_string))
				return ERR_SYNTAX;
			return OK;

		case TOKEN_ASSIGN:		// <identifier> -> #identifier = <expression>
			CURRENT_TOKEN = get_next_token(buffer_string);
			return precedent(buffer_string);

		default:
			return ERR_SYNTAX;
	}
}

Error ll_return(BufferString* buffer_string){
	ll_log("ll_return");
	switch(CURRENT_TOKEN){
		case TOKEN_EOL:		//<return> -> return
		case TOKEN_EOF:
		case TOKEN_PARENTHESIS_RIGHT:
		case TOKEN_BRACE_RIGHT:
			return OK;

		default:			//<return> -> return <expression>
			return precedent(buffer_string);	
	}
}

Error ll_statement(BufferString* buffer_string){
	ll_log("ll_statement");
	switch(CURRENT_TOKEN){
		case TOKEN_EOL:				//<statement> -> EOL
			CURRENT_TOKEN = get_next_token(buffer_string);

		case TOKEN_BRACE_RIGHT:		//<statement> -> ɛ
		case TOKEN_KEYWORD_FUNC:
			return OK;

		case TOKEN_KEYWORD_IF:		//<statement> -> <if>
			CURRENT_TOKEN = get_next_token(buffer_string);
			return ll_if(buffer_string);

		case TOKEN_KEYWORD_WHILE:	//<statement> -> <while>
			CURRENT_TOKEN = get_next_token(buffer_string);
			return ll_while(buffer_string);

		case TOKEN_KEYWORD_RETURN:	//<statement> -> <return>
			CURRENT_TOKEN = get_next_token(buffer_string);
			return ll_return(buffer_string);

		case TOKEN_IDENTIFIER:		//<statement> -> <identifier>
			CURRENT_TOKEN = get_next_token(buffer_string);
			return ll_identifier(buffer_string);

		case TOKEN_KEYWORD_LET:		//<statement> -> let #identifier <assign_type>
		case TOKEN_KEYWORD_VAR:		//<statement> -> var #identifier <assign_type>
		//case TOKEN_KEYWORD_NIL:		//not sure about that
			CURRENT_TOKEN = get_next_token(buffer_string);
			if(CURRENT_TOKEN != TOKEN_IDENTIFIER)
				return ERR_SYNTAX;

			CURRENT_TOKEN = get_next_token(buffer_string);
			return ll_assign_type(buffer_string);

		default:
			return ERR_SYNTAX;
	}
}

Error ll_statements(BufferString* buffer_string){
	ll_log("ll_statements");
	switch(CURRENT_TOKEN){
		case TOKEN_BRACE_RIGHT:	//<statements> -> ɛ
		case TOKEN_EOF:
		case TOKEN_KEYWORD_FUNC:
			return OK;

		case TOKEN_EOL:		//<statements> -> EOL<statement><statements>
			CURRENT_TOKEN = get_next_token(buffer_string);

		default:				//<statements> -> <statement><statements>
			if(ll_statement(buffer_string))
				return ERR_SYNTAX;

			return ll_statements(buffer_string);
	}
}
