#include <stdlib.h>
#include "syntax_ll.h"
#include "syntax_precedent.h"
#include "lexer.h"


Error ERR;


Error ll_program(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	switch (CURRENT_TOKEN){
		// This will be skipped for simplicity, because GET_TOKEN() skips EOL
		case TOKEN_EOL: // <program> -> TOKEN_EOL <program>
			return ll_program(buffer_string, tree);

		case TOKEN_EOF: // <program> -> TOKEN_EOF
			return OK;

		case TOKEN_KEYWORD_FUNC: // <program> -> <func_def> <program>
			unget_token();
			ERR = ll_func_definition(buffer_string, (ASTNode**)&(ASTNode_find_leftmost_node(*tree)->a));
			return ERR ? ERR : ll_program(buffer_string, tree);

		default: //<program> -> <statements> <program>
			unget_token();
			ERR = ll_statements(buffer_string, tree);
			return ERR ? ERR : ll_program(buffer_string, tree);
	}
}


Error ll_func_definition(BufferString* buffer_string, ASTNode** tree){
	*tree = ASTNode_new(FUNC_DEFS);
	if (*tree == NULL)
		return ERR_INTERNAL;
	
	(*tree)->b = ASTNode_new(FUNC_DEF);
	if ((*tree)->b == NULL)
		return ERR_INTERNAL;
	
	tree = (ASTNode**)&((*tree)->b); // tree is now FUNC_DEF

	ERR = ll_func_definition_head(buffer_string, (ASTNode**)&((*tree)->a));
	if (ERR)
		return ERR;	
	
	return ll_func_definition_body(buffer_string, (ASTNode**)&((*tree)->b));
}


Error ll_func_definition_head(BufferString* buffer_string, ASTNode** func_head){	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_FUNC)
		return ERR_SYNTAX;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	// create FUNC_HEAD
	*func_head = ASTNode_new(FUNC_HEAD);
	if (func_head == NULL)
		return ERR_INTERNAL;
	// create FUNC_HEAD_SIGNATURE
	ASTNode* func_signature = ASTNode_new(FUNC_HEAD_SIGNATURE);
	if (func_signature == NULL)
		return ERR_INTERNAL;
	(*func_head)->a = (void*)func_signature;

	// assing func name
	func_signature->a = (void*)BufferString_get_as_string(buffer_string);
	if (func_signature->a == NULL)
		return ERR_INTERNAL;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	
	// assign func args
	ERR = ll_func_definition_head_args(buffer_string, (FuncDefArg**)(&(func_signature->b)));
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
		func_signature->b = (void*)TYPE_INT;
		break;
	case TOKEN_KEYWORD_DOUBLE:
		func_signature->b = (void*)TYPE_DOUBLE;
		break;
	case TOKEN_KEYWORD_STRING:
		func_signature->b = (void*)TYPE_STRING;
		break;
	// case (more types here)
	default:
		return ERR_SYNTAX;
	}

	return OK;
}


Error ll_func_definition_head_args(BufferString* buffer_string, FuncDefArg** func_args){
	GET_TOKEN(true);
	unget_token();

	if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT)
		return OK;

	do {
		// check arg
		ERR = ll_func_definition_head_arg(buffer_string, FuncDefArg_get_last_arg(func_args));
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


Error ll_func_definition_head_arg(BufferString* buffer_string, FuncDefArg** func_arg){
	*func_arg = FuncDefArg_new();
	if (*func_arg == NULL)
		return ERR_INTERNAL;
	
	// arg name
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	
	// assing arg name
	(*func_arg)->name = BufferString_get_as_string(buffer_string);
	if ((*func_arg)->name == NULL)
		return ERR_INTERNAL;
	
	// name can be "_" meaning without name
	if ((*func_arg)->name[0] == '_' && (*func_arg)->name[1] == '\0')
		(*func_arg)->name = NULL;
	
	// arg identifier
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	
	// assing arg identifier
	(*func_arg)->identifier = BufferString_get_as_string(buffer_string);
	if ((*func_arg)->identifier == NULL)
		return ERR_INTERNAL;
	
	// identifier cannot be just "_" ("__" if fine tho)
	if ((*func_arg)->identifier[0] == '_' && (*func_arg)->identifier[1] == '\0')
		return ERR_SYNTAX;

	// arg type
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_COLON)
		return ERR_SYNTAX;

	GET_TOKEN(true);
	switch (CURRENT_TOKEN){
	case TOKEN_KEYWORD_INT:
		(*func_arg)->type = TYPE_INT;
		break;
	case TOKEN_KEYWORD_DOUBLE:
		(*func_arg)->type = TYPE_DOUBLE;
		break;
	case TOKEN_KEYWORD_STRING:
		(*func_arg)->type = TYPE_STRING;
		break;
	// case (more types here)
	default:
		return ERR_SYNTAX;
	}

	return OK;
}


Error ll_func_definition_body(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;
	
	ERR = ll_statements(buffer_string, tree);
	if (ERR)
		return ERR;
  
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;
	
	return OK;
}


Error ll_statements(BufferString* buffer_string, ASTNode** tree){
	char* name; // helper variable to store TOKEN_IDENTIFIER contents

	*tree = ASTNode_new(STATEMENT);
	if (*tree == NULL)
		return ERR_INTERNAL;

	GET_TOKEN(true);
	switch (CURRENT_TOKEN){
		case TOKEN_KEYWORD_VAR: // <statements> -> <var_declaration> <statements>
			unget_token();
			ERR = ll_var_declaration(buffer_string, (ASTNode**)(&((*tree)->a)));
			return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
		
		case TOKEN_KEYWORD_LET: // <statements> -> <let_declaration> <statements>
			unget_token();
			ERR = ll_let_declaration(buffer_string, (ASTNode**)(&((*tree)->a)));
			return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));

		case TOKEN_KEYWORD_IF: // <statements> -> <if> <statements>
			unget_token();
			ERR = ll_if(buffer_string, (ASTNode**)(&((*tree)->a)));
			return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
		
		case TOKEN_KEYWORD_WHILE: // <statements> -> <while> <statements>
			unget_token();
			ERR = ll_while(buffer_string, (ASTNode**)(&((*tree)->a)));
			return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
		
		case TOKEN_KEYWORD_RETURN: // <statements> -> <return> <statements>
			unget_token();
			ERR = ll_return(buffer_string, (ASTNode**)(&((*tree)->a)));
			return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
		
		case TOKEN_IDENTIFIER: // <statements> -> <assing> <statements> || <statements> -> <func_call> <statements>
			// as we can't go back, we need to store the identifier now
			name = BufferString_get_as_string(buffer_string);
			if (name == NULL)
				return ERR_INTERNAL;
			
			GET_TOKEN(true); // either TOKEN_ASSING or TOKEN_PARENTHESIS_LEFT
			unget_token();
			if (CURRENT_TOKEN == TOKEN_ASSIGN){
				ERR = ll_assign(buffer_string, (ASTNode**)(&((*tree)->a)), name);
				return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
			}

			else if (CURRENT_TOKEN == TOKEN_PARENTHESIS_LEFT){
				ERR = ll_func_call(buffer_string, (ASTNode**)(&((*tree)->a)), name);
				return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
			}

			else
				return ERR_SYNTAX;
		
		default: // <statement> -> E
			unget_token();
			return OK;
	}
}


Error ll_func_call(BufferString* buffer_string, ASTNode** tree, char* func_name){
	*tree = ASTNode_new(FUNC_CALL);
	if (*tree == NULL)
		return ERR_INTERNAL;
	
	// assign func name
	(*tree)->a = (void*)func_name;

	// we cannot start from the identifier as it was unreversibly consumed before
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	
	ERR = ll_func_call_args(buffer_string, (ASTNode**)((*tree)->b));
	if (ERR)
		return ERR;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;
	
	return OK;
}


// // foo(with: x, y)
// // foo(x, y)
// // foo()
Error ll_func_call_args(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	unget_token();
	if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT) // no args
		return OK;

	// Look mom! I am using a do-while loop!
	do{
		*tree = ASTNode_new(FUNC_CALL_ARGS);
		if (*tree == NULL)
			return ERR_INTERNAL;
		
		ERR = ll_func_call_arg(buffer_string, (ASTNode**)(&((*tree)->a)));
		if (ERR)
			return ERR;
		
		tree = (ASTNode**)(&((*tree)->b)); // tree is now the last FUNC_CALL_ARGS pointer
		
		GET_TOKEN(true);
	} while(CURRENT_TOKEN == TOKEN_COMMA);
	
	unget_token();
	return OK;
}


// // foo(x: val,)
// // foo(val,)
Error ll_func_call_arg(BufferString* buffer_string, ASTNode** tree){
	ERR = save_current_file_offset(); // save our position at the start of the current arg
	if (ERR)
		return ERR;

	// call GET_TOKEN() 2x because we want to look two tokens ahead to decide how to process the arg
	GET_TOKEN(true);
	GET_TOKEN(true);
	// immediately go back to the first token of the arg
	ERR = rollback_to_saved_file_offset();
	if (ERR)
		return ERR;
	
	*tree = ASTNode_new(FUNC_CALL_ARG);
	if (*tree == NULL)
		return ERR_INTERNAL;

	// CURRENT_TOKEN is still the value from the second calling of GET_TOKEN()
	if (CURRENT_TOKEN == TOKEN_COLON) // arg with name
		return ll_func_call_arg_with_name(buffer_string, *tree);
	else // arg without name
		return ll_func_call_arg_without_name(buffer_string, *tree);
}


Error ll_func_call_arg_with_name(BufferString* buffer_string, ASTNode* tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	
	tree->a = (void*)BufferString_get_as_string(buffer_string);
	if (tree->a == NULL)
		return ERR_INTERNAL;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_COLON)
		return ERR_SYNTAX;
	
	// TODO precedence get value
	return OK;
}

Error ll_func_call_arg_without_name(BufferString* buffer_string, ASTNode* tree){















	// TODO precedence get value
	return OK;
}


Error ll_assign(BufferString* buffer_string, ASTNode** tree, char* var_name){
	*tree = ASTNode_new(ASSIGN);
	if (*tree == NULL)
		return ERR_INTERNAL;

	(*tree)->a = var_name;

	// we cannot start from identifier
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_ASSIGN)
		return ERR_SYNTAX;

	// TODO precendence get value

	return OK;
}

// var id: type = exp
// var id: = exp
// var id: type
Error ll_let_var_declaration(BufferString* buffer_string, ASTNode* tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	// create subtrees
	tree->a = (void*)ASTNode_new(VAR_TYPE);
	if (tree->a == NULL)
		return ERR_INTERNAL;
	tree->b = (void*)ASTNode_new(VAR_HEAD);
	if (tree->b == NULL)
		return ERR_INTERNAL;
	// assign name
	((ASTNode*)(tree->b))->a = (void*)BufferString_get_as_string(buffer_string);
	if (((ASTNode*)(tree->b))->a == NULL)
		return ERR_INTERNAL;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_COLON)
		return ERR_SYNTAX;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_ASSIGN){
		((ASTNode*)(tree->a))->a = (void*)TYPE_NIL; // assign type
		((ASTNode*)(tree->a))->b = (void*)false; // assign nilable

		// assign expression
		return precedent(buffer_string, (exp_node**)(&((ASTNode*)(tree->b))->b));
	}

	// assign type
	switch (CURRENT_TOKEN){
		case TOKEN_KEYWORD_INT:
			((ASTNode*)(tree->a))->a = (void*)TYPE_INT;
			break;
		case TOKEN_KEYWORD_DOUBLE:
			((ASTNode*)(tree->a))->a = (void*)TYPE_DOUBLE;
			break;
		case TOKEN_KEYWORD_STRING:
			((ASTNode*)(tree->a))->a = (void*)TYPE_STRING;
			break;
		default:
			return ERR_SYNTAX;
	}


	((ASTNode*)(tree->a))->b = (void*)false; // assign nilable
	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_QUESTION){
		((ASTNode*)(tree->a))->b = (void*)true; // assign nilable override
		GET_TOKEN(true);
	}

	if (CURRENT_TOKEN != TOKEN_ASSIGN)
		return ERR_SYNTAX;
	
	// assign expression
	return precedent(buffer_string, (exp_node**)(&((ASTNode*)(tree->b))->b));
}


Error ll_var_declaration(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_VAR)
		return ERR_SYNTAX;

	*tree = ASTNode_new(VAR_DEF);
	if (*tree == NULL)
		return ERR_INTERNAL;

	return ll_let_var_declaration(buffer_string, *tree);
}

Error ll_let_declaration(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_LET)
		return ERR_SYNTAX;

	*tree = ASTNode_new(LET_DEF);
	if (*tree == NULL)
		return ERR_INTERNAL;

	return ll_let_var_declaration(buffer_string, *tree);
}

Error ll_while(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_WHILE)
		return ERR_SYNTAX;
	
	*tree = ASTNode_new(WHILE);
	if (*tree == NULL)
		return ERR_INTERNAL;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	
	ERR = precedent(buffer_string, (exp_node**)(&((*tree)->a)));
	if (ERR)
		return ERR;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		return ERR_SYNTAX;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;
	
	ERR = ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
	if (ERR)
		return ERR;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;

	return OK;
}

Error ll_return(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_RETURN)
		return ERR_SYNTAX;
	
	*tree = ASTNode_new(RETURN);
	if (*tree == NULL)
		return ERR_INTERNAL;
	
	return precedent(buffer_string, (exp_node**)(&((*tree)->a)));
}

Error ll_if(BufferString* buffer_string, ASTNode** tree){
	*tree = ASTNode_new(IFELSE);
	if (*tree == NULL)
		return ERR_INTERNAL;
	
	// put it in a helper variable so that we don't need to use that casting hell
	ASTNode* bodies = ASTNode_new(IFELSE_BODIES);
	if (bodies == NULL)
		return ERR_INTERNAL;	
	(*tree)->b = (void*)bodies;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_IF)
		return ERR_SYNTAX;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_KEYWORD_LET){
		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
			return ERR_SYNTAX;
		
		char* id = BufferString_get_as_string(buffer_string);
		if (id == NULL)
			return ERR_INTERNAL;

		ERR = let_nil((exp_node**)(&((*tree)->a)), id);
		if (ERR)
			return ERR;

		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
			return ERR_SYNTAX;
		
		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
			return ERR_SYNTAX;

		// TODO pre generated statement
		// bodies->a

		// find last statement
		// ERR = ll_statements(buffer_string, ));
		if (ERR)
			return ERR;

		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
			return ERR_SYNTAX;

	} else {
		ERR = precedent(buffer_string, (exp_node**)(&((*tree)->a)));
		if (ERR)
			return ERR;

		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
			return ERR_SYNTAX;

		ERR = ll_statements(buffer_string, (ASTNode**)(&(bodies->a)));
		if (ERR)
			return ERR;

		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
			return ERR_SYNTAX;
	}

	return ll_else(buffer_string, (ASTNode**)(&(bodies->b)));
}


// if (let x){}
// if (x != nil){
// let x := x
// }

Error generate_if_let_declaration_override_statement(ASTNode** tree, char* id){
	*tree = ASTNode_new(LET_DEF);
	if (*tree == NULL)
		return ERR_INTERNAL;

	(*tree)->a = ASTNode_new(VAR_TYPE);
	if ((*tree)->a == NULL)
		return ERR_INTERNAL;

	(*tree)->b = ASTNode_new(VAR_HEAD);
	if ((*tree)->b == NULL)
		return ERR_INTERNAL;

	((ASTNode*)((*tree)->a))->a = (void*)TYPE_NIL;
	((ASTNode*)((*tree)->a))->b = (void*)false;

	((ASTNode*)((*tree)->b))->a = (void*)id;
	// ((ASTNode*)((*tree)->b))->b = (void*);

	variable_expression((exp_node**)(&(((ASTNode*)((*tree)->b))->b)), id);
	
	return OK;
}

Error ll_else(BufferString* buffer_string, ASTNode** tree){
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_ELSE)
		return ERR_SYNTAX;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
		return ERR_SYNTAX;

	ERR = ll_statements(buffer_string, tree);
	if (ERR)
		return ERR;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
		return ERR_SYNTAX;
	
	return OK;
}
