#include "syntax_ll.h"


Token CURRENT_TOKEN;
Error ERR;



Error ll_program(BufferString* buffer_string, SymTable* table, ASTNode** tree){
	GET_TOKEN(true);
	switch (CURRENT_TOKEN){
		// This will be skipped for simplicity
		case TOKEN_EOL: // <program> -> TOKEN_EOL <program>
			return ll_program(buffer_string, table, tree);

		case TOKEN_EOF: // <program> -> TOKEN_EOF
			return OK;

		case TOKEN_KEYWORD_FUNC: // <program> -> <func_def> <program>
			unget_token();
			ERR = ll_func_definition(buffer_string, table, (ASTNode**)&(ASTNode_find_leftmost_node(*tree)->a));
			return ERR ? ERR : ll_program(buffer_string, table, tree);

		default:	//<program> -> <statements> <program>
			unget_token();
			ERR = ll_statements(buffer_string, tree);
			return ERR ? ERR : ll_program(buffer_string, table, tree);
	}
}


Error ll_func_definition(BufferString* buffer_string, SymTable* table, ASTNode** tree){
	*tree = ASTNode_new(FUNC_DEFS);
	if (*tree == NULL)
		return ERR_INTERNAL;
	
	(*tree)->b = ASTNode_new(FUNC_DEF);
	if ((*tree)->b == NULL)
		return ERR_INTERNAL;
	
	tree = (ASTNode**)&((*tree)->b); // tree is now FUNC_DEF

	ERR = ll_func_definition_head(buffer_string, table, (ASTNode**)&((*tree)->a));
	if (ERR)
		return ERR;
	
	return ll_func_definition_body(buffer_string, table, (ASTNode**)&((*tree)->b));
}


Error ll_func_definition_head(BufferString* buffer_string, SymTable* table, ASTNode** tree){	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_FUNC)
		return ERR_SYNTAX;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	
	// FUNC_DEF->a is name in symtable
	*tree = (void*)BufferString_get_as_string(buffer_string);
	if (*tree == NULL)
		return ERR_INTERNAL;
	// Write it to symtable
	Symbol* symbol = Symbol_new();
	if (symbol == NULL)
		return ERR_INTERNAL;
	symbol->name = (char*)*tree; // assign name to symbol
	ERR = SymTable_insert(table, symbol); // insert symbol into table
	if (ERR)
		return ERR;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	
	ERR = ll_func_definition_head_args(buffer_string, table, tree);
	if (ERR)
		return ERR;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;
	

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_ARROW){ // no return type specified. void function
		unget_token();
		return OK;
	}

	GET_TOKEN(true);
	switch (CURRENT_TOKEN){
	case TOKEN_KEYWORD_INT:
		symbol->type = INT;
		break;
	case TOKEN_KEYWORD_DOUBLE:
		symbol->type = DOUBLE;
		break;
	case TOKEN_KEYWORD_STRING:
		symbol->type = STRING;
		break;
	// case (more types here)
	default:
		return ERR_SYNTAX;
	}

	return OK;
}


Error ll_func_definition_head_args(BufferString* buffer_string, SymTable* table, ASTNode** tree){
	GET_TOKEN(true);
	unget_token();

	if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT)
		return OK;

	do {
		// check arg
		ERR = ll_func_definition_head_arg(buffer_string, table, tree);
		if (ERR)
			return ERR;

		// check arg closing
		GET_TOKEN(true);
		if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT){
			unget_token();
			return OK;
		}

	} while(CURRENT_TOKEN == TOKEN_COMMA); // if comma is found, next must be next arg
	// anything else is error
	return ERR_SYNTAX;
}

Error ll_func_definition_head_arg(BufferString* buffer_string, SymTable* table, ASTNode** tree){
	Arg* arg = Arg_new();
	if (arg == NULL)
		return ERR_INTERNAL;
	
	// arg name
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)	
		return ERR_SYNTAX;
	
	arg->name = BufferString_get_as_string(buffer_string);
	if (arg->name == NULL)
		return ERR_INTERNAL;
	// name can be _ meaning without name
	if (arg->name[0] == '_' && arg->name[1] == '\0')
		arg->name = NULL;
	
	// arg identifier
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	arg->identifier = BufferString_get_as_string(buffer_string);
	if (arg->identifier == NULL)
		return ERR_INTERNAL;
	// identifier cannot be just _ (__ if fine tho)
	if (arg->identifier[0] == '_' && arg->identifier[1] == '\0')
		return ERR_SYNTAX;

	// arg type
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_COLON)
		return ERR_SYNTAX;

	GET_TOKEN(true);
	switch (CURRENT_TOKEN){
	case TOKEN_KEYWORD_INT:
		arg->type = INT;
		break;
	case TOKEN_KEYWORD_DOUBLE:
		arg->type = DOUBLE;
		break;
	case TOKEN_KEYWORD_STRING:
		arg->type = STRING;
		break;
	// case (more types here)
	default:
		return ERR_SYNTAX;
	}

	// TODO waiting for SymTable rework so it accepts NULL as scope

	// now add the arg to the function symbol;
	// Symbol* symbol = SymTable_get(table, (char*)*tree, NULL); // segfault
	// if (symbol == NULL)
	// 	return ERR_INTERNAL;
	
	// Arg** free_arg_p = Symbol_get_free_arg_p(symbol);
	// *free_arg_p = arg;
	return OK;
}


Error ll_func_definition_body(BufferString* buffer_string, SymTable* table, ASTNode** tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;
	
	printf("function body syntax not implemented\n");
	return ERR_INTERNAL;
  
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;
	
	return OK;
}


Error ll_func_statements(BufferString* buffer_string, void* tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_KEYWORD_RETURN){ // <func_statements> -> <return> <func_statements>
		unget_token();
		return ll_return(buffer_string, tree);
	}
	
	return ll_statements(buffer_string, tree); // <func_statements> -> <statements> <func_statements>
}


Error ll_statements(BufferString* buffer_string, void* tree){
	GET_TOKEN(true);
	switch (CURRENT_TOKEN){
		case TOKEN_KEYWORD_VAR: // <statements> -> <var_declaration> <statements>
			unget_token();
			ERR = ll_var_declaration(buffer_string, tree);
			return ERR ? ERR : ll_statements(buffer_string, tree);
		
		case TOKEN_KEYWORD_LET: // <statements> -> <let_declaration> <statements>
			unget_token();
			ERR = ll_let_declaration(buffer_string, tree);
			return ERR ? ERR : ll_statements(buffer_string, tree);
		
		case TOKEN_KEYWORD_IF: // <statements> -> <if> <statements>
			unget_token();
			ERR = ll_if(buffer_string, tree);
			return ERR ? ERR : ll_statements(buffer_string, tree);
		
		case TOKEN_KEYWORD_WHILE: // <statements> -> <while> <statements>
			unget_token();
			ERR = ll_while(buffer_string, tree);
			return ERR ? ERR : ll_statements(buffer_string, tree);
		
		case TOKEN_IDENTIFIER: // <statements> -> <assing> <statements> || <statements> -> <func_call> <statements>
			// we don't know where we'll go so save in case we need to go back
			ERR = save_current_file_offset();
			if (ERR)
				return ERR;
			
			GET_TOKEN(true); // either TOKEN_ASSING or TOKEN_PARENTHESIS_LEFT
			ERR = rollback_to_saved_file_offset(); // either way we want to go back

			if (CURRENT_TOKEN == TOKEN_ASSIGN)
				return ERR ? ERR : ll_assign(buffer_string, tree);

			else if (CURRENT_TOKEN == TOKEN_PARENTHESIS_LEFT)
				return ERR ? ERR : ll_func_call(buffer_string, tree);

			else
				return ERR_SYNTAX;
		
		// case
			
		default: // <statement> -> E
			unget_token();
			return OK;
	}
}


Error ll_func_call(BufferString* buffer_string, void* tree){
	GET_TOKEN(true);
	if(CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	char* name = BufferString_get_as_string(buffer_string);
	if (name == NULL)
		return ERR_INTERNAL;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	
	// add name to tree here so that args can be passed next to it

	ERR = ll_func_call_args(buffer_string, tree);
	if (ERR)
		return ERR;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;
	
	return OK;
}


// foo(with: x, y)
// foo(x, y)
// foo()
Error ll_func_call_args(BufferString* buffer_string, void* tree){
	// ERR = save_current_file_offset();
	// if (ERR)
	// 	return ERR;

	GET_TOKEN(true);
	unget_token();
	if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT) // no args
		return OK;

	do{
		ERR = ll_func_call_arg(buffer_string, tree);
		if (ERR)
			return ERR;
		
		GET_TOKEN(true);
	} while(CURRENT_TOKEN == TOKEN_COMMA);
	
	unget_token();
	return OK;
}


// foo(x: val,)
// foo(val,)
Error ll_func_call_arg(BufferString* buffer_string, void* tree){
	ERR = save_current_file_offset();
	if (ERR)
		return ERR;

	GET_TOKEN(true);
	GET_TOKEN(true);
	ERR = rollback_to_saved_file_offset();
	if (ERR)
		return ERR;

	if (CURRENT_TOKEN == TOKEN_COLON)
		return ll_func_call_arg_with_name(buffer_string, tree);
	
	return ll_func_call_arg_without_name(buffer_string, tree);
}


Error ll_func_call_arg_with_name(BufferString* buffer_string, void* tree){
	GET_TOKEN(true);

	char* name = BufferString_get_as_string(buffer_string);
	if (name == NULL)
		return ERR_INTERNAL;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_COLON)
		return ERR_SYNTAX;
	
	// TODO precedence get value
	return OK;
}

Error ll_func_call_arg_without_name(BufferString* buffer_string, void* tree){
	// TODO precedence get value
	return OK;
}


Error ll_assign(BufferString* buffer_string, void* tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	char* name = BufferString_get_as_string(buffer_string);
	if (name == NULL)
		return ERR_INTERNAL;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_ASSIGN)
		return ERR_SYNTAX;

	// TODO with precedence

	return OK;
}


Error ll_var_declaration(BufferString* buffer_string, void* tree){
	return OK;
}

Error ll_let_declaration(BufferString* buffer_string, void* tree){
	return OK;
}

Error ll_while(BufferString* buffer_string, void* tree){
	return OK;
}

Error ll_return(BufferString* buffer_string, void* tree){
	return OK;
}

Error ll_if(BufferString* buffer_string, void* tree){
	return OK;
}


/*
Error ll_while(BufferString* buffer_string){
	ll_log("ll_while");
	if(ll_while_head(buffer_string))     // while <expression>
		return ERR_SYNTAX;

	if(ll_statement_body(buffer_string)) // {<statement_body>}
		return ERR_SYNTAX;

    if(CURRENT_TOKEN != TOKEN_EOL && CURRENT_TOKEN != TOKEN_EOF)  // EOL
        return ERR_SYNTAX;

    CURRENT_TOKEN = get_token(buffer_string, true);
    return OK;
}

Error ll_while_head(BufferString* buffer_string){
	ll_log("ll_while_head");
	if(precedent(buffer_string))
		return ERR_SYNTAX;

	return OK;
}

Error ll_if(BufferString* buffer_string){
	ll_log("ll_if");
	if(ll_if_head(buffer_string))   // if <expression>
		return ERR_SYNTAX;

	if(ll_statement_body(buffer_string))	// {<statement_body>}
		return ERR_SYNTAX;

    if(CURRENT_TOKEN == TOKEN_EOL)      // EOL?
        CURRENT_TOKEN = get_token(buffer_string, true);

	if(CURRENT_TOKEN != TOKEN_KEYWORD_ELSE) // else
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, true);
	if(ll_statement_body(buffer_string))	// {<statement_body>}
		return ERR_SYNTAX;

    if(CURRENT_TOKEN != TOKEN_EOL && CURRENT_TOKEN != TOKEN_EOF)  // EOL
        return ERR_SYNTAX;

    CURRENT_TOKEN = get_token(buffer_string, true);
	return OK;
}

Error ll_if_head(BufferString* buffer_string){
	ll_log("ll_if_head");
	switch(CURRENT_TOKEN){
		case TOKEN_KEYWORD_LET:
		case TOKEN_KEYWORD_VAR:
			CURRENT_TOKEN = get_token(buffer_string, true);
			if(CURRENT_TOKEN != TOKEN_IDENTIFIER)
				return ERR_SYNTAX;

			CURRENT_TOKEN = get_token(buffer_string, true);
		default:
			return precedent(buffer_string);
	}
}

Error ll_func_definition(BufferString* buffer_string){
	ll_log("ll_func_definition");
	if(ll_func_head(buffer_string))
		return ERR_SYNTAX;
	
	if(ll_statement_body(buffer_string))
		return ERR_SYNTAX;

	if(CURRENT_TOKEN != TOKEN_EOL && CURRENT_TOKEN != TOKEN_EOF)  // EOL
		return ERR_SYNTAX;

    CURRENT_TOKEN = get_token(buffer_string, true);
    return OK;
}

Error ll_func_head(BufferString* buffer_string){
	ll_log("ll_func_head");
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, true);
	if(ll_func_params(buffer_string))
		return ERR_SYNTAX;


	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, true);
	if(ll_func_type(buffer_string))
		return ERR_SYNTAX;

	return OK;
}

Error ll_func_params(BufferString* buffer_string){
	ll_log("ll_func_params");
	if(CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT){
		CURRENT_TOKEN = get_token(buffer_string, true);
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

	CURRENT_TOKEN = get_token(buffer_string, true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, true);
	if (CURRENT_TOKEN != TOKEN_COLON)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, true);
	return ll_type(buffer_string);
}


Error ll_func_more_params(BufferString* buffer_string){
	ll_log("ll_func_more_params");
	switch(CURRENT_TOKEN){	// <func_more_params> -> ɛ
		case TOKEN_PARENTHESIS_RIGHT:
			return OK;

		case TOKEN_COMMA:	// <func_more_params> -> ,<func_param><func_more_params>
			CURRENT_TOKEN = get_token(buffer_string, true);
			if(ll_func_param(buffer_string))
				return ERR_SYNTAX;

			return ll_func_more_params(buffer_string);

		default:
			return ERR_SYNTAX;
	}
}

Error ll_func_type(BufferString* buffer_string){
	ll_log("ll_func_type");
	if (CURRENT_TOKEN != TOKEN_ARROW){
		return OK;
	}
	CURRENT_TOKEN = get_token(buffer_string, true);
	return ll_type(buffer_string);
}

Error ll_statement_body(BufferString* buffer_string){
	ll_log("ll_statement_body");
	if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, true);
	if(ll_statements(buffer_string))
		return ERR_SYNTAX;

	if(CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, false);    // v CURRENT TOKEN bude token tesne za brace_right
	return OK;
}

Error ll_program(BufferString* buffer_string){
	ll_log("ll_program");
	switch(CURRENT_TOKEN){
		case TOKEN_EOF:		//<program> -> ɛ
			return OK;

		// case TOKEN_EOL:		//<program> -> EOL<program>
		// 	CURRENT_TOKEN = get_token(buffer_string, true);
		// 	return	ll_program(buffer_string);

		case TOKEN_KEYWORD_FUNC:	//<program> -> <func_def>
			CURRENT_TOKEN = get_token(buffer_string, true);
			if(ll_func_definition(buffer_string))
				return ERR_SYNTAX;

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

	CURRENT_TOKEN = get_token(buffer_string, true);
	if(ll_func_args(buffer_string))
		return ERR_SYNTAX;

	if(CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;

	CURRENT_TOKEN = get_token(buffer_string, false);
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
			CURRENT_TOKEN = get_token(buffer_string, true);
			if(CURRENT_TOKEN == TOKEN_COLON){
				CURRENT_TOKEN = get_token(buffer_string, true);
			}
			else{
				return OK;
			}

			if(CURRENT_TOKEN == TOKEN_IDENTIFIER){
				CURRENT_TOKEN = get_token(buffer_string, true);
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
			CURRENT_TOKEN = get_token(buffer_string, true);
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
			CURRENT_TOKEN = get_token(buffer_string, false);
			if(CURRENT_TOKEN == TOKEN_QUESTION)
				CURRENT_TOKEN = get_token(buffer_string, true);

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
			CURRENT_TOKEN = get_token(buffer_string, true);
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
			CURRENT_TOKEN = get_token(buffer_string, true);
			return OK;

		default:
			return ERR_SYNTAX;
	}
}

Error ll_assign(BufferString* buffer_string){
	ll_log("ll_assign");
	switch(CURRENT_TOKEN){
		case TOKEN_ASSIGN:	//<assign> -> = <expressions>
			CURRENT_TOKEN = get_token(buffer_string, true);
			return precedent(buffer_string);

		default:		//<assign> -> ɛ :
			return OK;
	}
}

Error ll_assign_type(BufferString* buffer_string){
	ll_log("ll_assign_type");
	switch(CURRENT_TOKEN){	// <assign_type> -> : <type_guestion> <assign>
		case TOKEN_COLON:
			CURRENT_TOKEN = get_token(buffer_string, true);
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
			CURRENT_TOKEN = get_token(buffer_string, true);
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
			CURRENT_TOKEN = get_token(buffer_string, true);

		case TOKEN_BRACE_RIGHT:		//<statement> -> ɛ
		case TOKEN_KEYWORD_FUNC:
			return OK;

		case TOKEN_KEYWORD_IF:		//<statement> -> <if>
			CURRENT_TOKEN = get_token(buffer_string, true);
			return ll_if(buffer_string);

		case TOKEN_KEYWORD_WHILE:	//<statement> -> <while>
			CURRENT_TOKEN = get_token(buffer_string, true);
			return ll_while(buffer_string);

		case TOKEN_KEYWORD_RETURN:	//<statement> -> <return>
			CURRENT_TOKEN = get_token(buffer_string, true);
			return ll_return(buffer_string);

		case TOKEN_IDENTIFIER:		//<statement> -> <identifier>
			CURRENT_TOKEN = get_token(buffer_string, true);
			return ll_identifier(buffer_string);

		case TOKEN_KEYWORD_LET:		//<statement> -> let #identifier <assign_type>
		case TOKEN_KEYWORD_VAR:		//<statement> -> var #identifier <assign_type>
			CURRENT_TOKEN = get_token(buffer_string, true);
			if(CURRENT_TOKEN != TOKEN_IDENTIFIER)
				return ERR_SYNTAX;

			CURRENT_TOKEN = get_token(buffer_string, true);
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
		case TOKEN_KEYWORD_FUNC:		// dovoluje definici funkci v tele jine funkce -> nechal bych to na semantice
			return OK;

		case TOKEN_EOL:		//<statements> -> EOL<statement><statements>
			CURRENT_TOKEN = get_token(buffer_string, true);

		default:				//<statements> -> <statement><statements>
			if(ll_statement(buffer_string))
				return ERR_SYNTAX;

			return ll_statements(buffer_string);
	}
}

*/