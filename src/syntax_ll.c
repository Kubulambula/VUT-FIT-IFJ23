#include <stdlib.h>
#include <string.h>
#include "syntax_ll.h"
#include "syntax_precedent.h"
#include "lexer.h"


Error syntax(ASTNode** tree){
	BufferString buffer_string;
	if(!BufferString_init(&buffer_string))
		return ERR_INTERNAL;

	*tree = ASTNode_new(ROOT);
	if (*tree == NULL){
		BufferString_free(&buffer_string);
		return ERR_INTERNAL;
	}

	ERR = generate_built_in_function_definitions(*tree);
	if (ERR)
		return ERR;

	ERR = ll_program(&buffer_string, *tree);
	BufferString_free(&buffer_string);
	if (ERR){
		ASTNode_free(*tree);
		*tree = NULL;
		return ERR;
	}

	return OK;
}


// add built in function to AST to make semantic life easier
Error generate_built_in_function_definitions(ASTNode* root){
	ERR = generate_built_in_readString((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_readInt((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_readDouble((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_write((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_Int2Double((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_Double2Int((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_length((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_substring((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_ord((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;
	ERR = generate_built_in_chr((ASTNode**)&(ASTNode_find_leftmost_node(root)->a));
	if (ERR)
		return ERR;

	return OK;
}


Error generate_blank_function(ASTNode** func_defs, ASTNode** func_head, ASTNode** func_head_signature, ASTNode** func_type){
	// create new FUNC_DEFS - add a function to LList of functions
	*func_defs = ASTNode_new(FUNC_DEFS);
	if (*func_defs == NULL)
		return ERR_INTERNAL;
	// create new FUNC_DEF - add a concrete function
	ASTNode* func_def = ASTNode_new(FUNC_DEF);
	if (func_def == NULL)
		return ERR_INTERNAL;
	(*func_defs)->b = func_def;
	// create new FUNC_HEAD - concrete function head
	*func_head = ASTNode_new(FUNC_HEAD);
	if (*func_head == NULL)
		return ERR_INTERNAL;
	func_def->a = (void*)(*func_head);
	// create new FUNC_SIGNATURE for the concrete function
	*func_head_signature = ASTNode_new(FUNC_HEAD_SIGNATURE);
	if (*func_head_signature == NULL)
		return ERR_INTERNAL;
	(*func_head)->a = (void*)(*func_head_signature);
	// create new VAR_TYPE - return type of the concrete function
	*func_type = ASTNode_new(VAR_TYPE);
	if (*func_type == NULL)
		return ERR_INTERNAL;
	((ASTNode*)(*func_head))->b = (void*)(*func_type);
	
	return OK;
}


Error generate_built_in_readString(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 11);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "readString"); // func name
	func_head_signature->b = NULL; // func args

	func_type->a = (void*)TYPE_STRING; // func type
	func_type->b = (void*)true; // func type nilable

	return OK;
}

Error generate_built_in_readInt(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 8);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "readInt"); // func name
	func_head_signature->b = NULL; // func args

	func_type->a = (void*)TYPE_INT; // func type
	func_type->b = (void*)true; // func type nilable

	return OK;
}

Error generate_built_in_readDouble(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 11);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "readDouble"); // func name
	func_head_signature->b = NULL; // func args

	func_type->a = (void*)TYPE_DOUBLE; // func type
	func_type->b = (void*)true; // func type nilable

	return OK;
}

Error generate_built_in_write(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 6);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "write"); // func name
	func_head_signature->b = NULL; // func args - write is an exeption and in symtable it is storead as it has no args

	func_type->a = (void*)TYPE_NIL; // func type
	func_type->b = (void*)false; // func type nilable

	return OK;
}

Error generate_built_in_Int2Double(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 11);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "Int2Double"); // func name

	func_head_signature->b = FuncDefArg_new(); // func args
	if (func_head_signature->b == NULL)
		return ERR_INTERNAL;
	
	((FuncDefArg*)(func_head_signature->b))->name = NULL; // arg name
	((FuncDefArg*)(func_head_signature->b))->identifier = malloc(sizeof(char) * 5);
	if (((FuncDefArg*)(func_head_signature->b))->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(((FuncDefArg*)(func_head_signature->b))->identifier, "term"); // arg identifier
	((FuncDefArg*)(func_head_signature->b))->type = TYPE_INT; // arg identifier
	((FuncDefArg*)(func_head_signature->b))->nilable = false; // arg identifier

	func_type->a = (void*)TYPE_DOUBLE; // func type
	func_type->b = (void*)false; // func type nilable

	return OK;
}

Error generate_built_in_Double2Int(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 11);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "Double2Int"); // func name

	func_head_signature->b = FuncDefArg_new(); // func args
	if (func_head_signature->b == NULL)
		return ERR_INTERNAL;
	
	((FuncDefArg*)(func_head_signature->b))->name = NULL; // arg name
	((FuncDefArg*)(func_head_signature->b))->identifier = malloc(sizeof(char) * 5);
	if (((FuncDefArg*)(func_head_signature->b))->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(((FuncDefArg*)(func_head_signature->b))->identifier, "term"); // arg identifier
	((FuncDefArg*)(func_head_signature->b))->type = TYPE_DOUBLE; // arg identifier
	((FuncDefArg*)(func_head_signature->b))->nilable = false; // arg identifier
	
	func_type->a = (void*)TYPE_INT; // func type
	func_type->b = (void*)false; // func type nilable

	return OK;
}

Error generate_built_in_length(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 7);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "length"); // func name

	func_head_signature->b = FuncDefArg_new(); // func args
	if (func_head_signature->b == NULL)
		return ERR_INTERNAL;
	
	((FuncDefArg*)(func_head_signature->b))->name = NULL; // arg name
	((FuncDefArg*)(func_head_signature->b))->identifier = malloc(sizeof(char) * 2);
	if (((FuncDefArg*)(func_head_signature->b))->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(((FuncDefArg*)(func_head_signature->b))->identifier, "s"); // arg identifier
	((FuncDefArg*)(func_head_signature->b))->type = TYPE_STRING; // arg identifier
	((FuncDefArg*)(func_head_signature->b))->nilable = false; // arg identifier
	

	func_type->a = (void*)TYPE_INT; // func type
	func_type->b = (void*)false; // func type nilable

	return OK;
}

Error generate_built_in_substring(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 10);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "substring"); // func name

	FuncDefArg* arg1 = FuncDefArg_new();
	if (arg1 == NULL)
		return ERR_INTERNAL;
	func_head_signature->b = arg1; // add 1st arg
	
	arg1->name = malloc(sizeof(char) * 3);
	if (arg1->name == NULL)
		return ERR_INTERNAL;
	strcpy(arg1->name, "of"); // arg1 name
	arg1->identifier = malloc(sizeof(char) * 2);
	if (arg1->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(arg1->identifier, "s"); // arg1 identifier
	arg1->type = TYPE_STRING; // arg1 identifier
	arg1->nilable = false; // arg1 identifier

	FuncDefArg* arg2 = FuncDefArg_new();
	if (arg2 == NULL)
		return ERR_INTERNAL;
	arg1->next = arg2; // add 2nd arg

	arg2->name = malloc(sizeof(char) * 11);
	if (arg2->name == NULL)
		return ERR_INTERNAL;
	strcpy(arg2->name, "startingAt"); // arg2 name
	arg2->identifier = malloc(sizeof(char) * 2);
	if (arg2->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(arg2->identifier, "i"); // arg2 identifier
	arg2->type = TYPE_INT; // arg2 identifier
	arg2->nilable = false; // arg2 identifier

	FuncDefArg* arg3 = FuncDefArg_new();
	if (arg3 == NULL)
		return ERR_INTERNAL;
	arg2->next = arg3; // add 3rd arg

	arg3->name = malloc(sizeof(char) * 13);
	if (arg3->name == NULL)
		return ERR_INTERNAL;
	strcpy(arg3->name, "endingBefore"); // arg3 name
	arg3->identifier = malloc(sizeof(char) * 2);
	if (arg3->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(arg3->identifier, "j"); // arg3 identifier
	arg3->type = TYPE_INT; // arg3 type
	arg3->nilable = false; // arg3 nilable

	func_type->a = (void*)TYPE_STRING; // func type
	func_type->b = (void*)true; // func type nilable

	return OK;
}

Error generate_built_in_ord(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 4);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "ord"); // func name

	func_head_signature->b = FuncDefArg_new(); // func args
	if (func_head_signature->b == NULL)
		return ERR_INTERNAL;
	
	((FuncDefArg*)(func_head_signature->b))->name = NULL; // arg name
	((FuncDefArg*)(func_head_signature->b))->identifier = malloc(sizeof(char) * 2);
	if (((FuncDefArg*)(func_head_signature->b))->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(((FuncDefArg*)(func_head_signature->b))->identifier, "c"); // arg identifier
	((FuncDefArg*)(func_head_signature->b))->type = TYPE_STRING; // arg type
	((FuncDefArg*)(func_head_signature->b))->nilable = false; // arg nilable
	

	func_type->a = (void*)TYPE_INT; // func type
	func_type->b = (void*)false; // func type nilable

	return OK;
}

Error generate_built_in_chr(ASTNode** func_defs){
	ASTNode* func_head;
	ASTNode* func_head_signature;
	ASTNode* func_type;
	ERR = generate_blank_function(func_defs, &func_head, &func_head_signature, &func_type);
	if (ERR)
		return ERR;
	// populate the blank function
	func_head_signature->a = malloc(sizeof(char) * 4);
	if (func_head_signature->a == NULL)
		return ERR_INTERNAL;
	strcpy(func_head_signature->a, "chr"); // func name

	func_head_signature->b = FuncDefArg_new(); // func args
	if (func_head_signature->b == NULL)
		return ERR_INTERNAL;
	
	((FuncDefArg*)(func_head_signature->b))->name = NULL; // arg name
	((FuncDefArg*)(func_head_signature->b))->identifier = malloc(sizeof(char) * 2);
	if (((FuncDefArg*)(func_head_signature->b))->identifier == NULL)
		return ERR_INTERNAL;
	strcpy(((FuncDefArg*)(func_head_signature->b))->identifier, "i"); // arg identifier
	((FuncDefArg*)(func_head_signature->b))->type = TYPE_INT; // arg identifier
	((FuncDefArg*)(func_head_signature->b))->nilable = false; // arg identifier
	

	func_type->a = (void*)TYPE_STRING; // func type
	func_type->b = (void*)false; // func type nilable

	return OK;
}




Error ll_program(BufferString* buffer_string, ASTNode* tree){
	GET_TOKEN(true);
	unget_token(); // unget the token because the next function has to read it again
	switch (CURRENT_TOKEN){
		// This will be skipped for simplicity, because GET_TOKEN() skips EOL
		case TOKEN_EOL: // <program> -> TOKEN_EOL <program>
			return ll_program(buffer_string, tree);

		case TOKEN_EOF: // <program> -> TOKEN_EOF
			return OK;

		case TOKEN_KEYWORD_FUNC: // <program> -> <func_def> <program>
			ERR = ll_func_definition(buffer_string, (ASTNode**)&(ASTNode_find_leftmost_node(tree)->a));
			return ERR ? ERR : ll_program(buffer_string, tree);

		//<program> -> <statements> <program>
		// We have to check for the possible ll_statements states beforehand so we don't end up in an infinite loop
		case TOKEN_KEYWORD_VAR:
		case TOKEN_KEYWORD_LET:
		case TOKEN_KEYWORD_IF:
		case TOKEN_KEYWORD_WHILE:
		case TOKEN_KEYWORD_RETURN:
		case TOKEN_IDENTIFIER:
			ERR = ll_statements(buffer_string, (ASTNode**)&(ASTNode_find_rightmost_node(tree)->b));
			return ERR ? ERR : ll_program(buffer_string, tree);
		
		default:
			print_token_as_string(CURRENT_TOKEN);
			return ERR_SYNTAX;
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
	// create VAR_TYPE - for return type
	ASTNode* var_type = ASTNode_new(VAR_TYPE);
	if (var_type == NULL)
		return ERR_INTERNAL;
	(*func_head)->b = (void*)var_type;
	var_type->a = (void*)TYPE_NIL;
	var_type->b = (void*)false;

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
		var_type->a = (void*)TYPE_INT;
		break;
	case TOKEN_KEYWORD_DOUBLE:
		var_type->a = (void*)TYPE_DOUBLE;
		break;
	case TOKEN_KEYWORD_STRING:
		var_type->a = (void*)TYPE_STRING;
		break;
	// case (more types here)
	default:
		return ERR_SYNTAX;
	}

	GET_TOKEN(true);
	if(CURRENT_TOKEN != TOKEN_QUESTION){
		unget_token();
		return OK;
	}

	var_type->b = (void*)true;
	return OK;
}


Error ll_func_definition_head_args(BufferString* buffer_string, FuncDefArg** func_args){
	GET_TOKEN(true);
	unget_token();

	if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT)
		return OK;

	FuncDefArg** arg = func_args;
	do {
		// check arg
		ERR = ll_func_definition_head_arg(buffer_string, arg);
		if (ERR)
			return ERR;

		// check arg closing
		GET_TOKEN(true);
		if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT){
			unget_token();
			return OK;
		}

		arg = &((*arg)->next);

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
	if ((*func_arg)->name[0] == '_' && (*func_arg)->name[1] == '\0'){
		free((*func_arg)->name);
		(*func_arg)->name = NULL;
	}
	
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

	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_QUESTION)
		(*func_arg)->nilable = true;
	else
		unget_token();

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

			// magic to free name if an error occurs inside lexer
			CURRENT_TOKEN = get_token(buffer_string, true);
			if (CURRENT_TOKEN == TOKEN_ERR_INTERNAL || CURRENT_TOKEN == TOKEN_ERR_LEXICAL){
				free(name);
				return CURRENT_TOKEN == TOKEN_ERR_INTERNAL ? ERR_INTERNAL : ERR_LEXICAL;
			}

			// GET_TOKEN(true); // either TOKEN_ASSING or TOKEN_PARENTHESIS_LEFT
			unget_token();
			if (CURRENT_TOKEN == TOKEN_ASSIGN){
				ERR = ll_assign(buffer_string, (ASTNode**)(&((*tree)->a)), name);
				return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
			}
			else if (CURRENT_TOKEN == TOKEN_PARENTHESIS_LEFT){
				ERR = ll_func_call(buffer_string, (ASTNode**)(&((*tree)->a)), name);
				return ERR ? ERR : ll_statements(buffer_string, (ASTNode**)(&((*tree)->b)));
			}

			free(name);
			return ERR_SYNTAX;
		
		default: // <statement> -> E
			// remove the empty STATEMENT
			free(*tree);
			*tree = NULL;

			unget_token();
			return OK;
	}
}


Error ll_func_call(BufferString* buffer_string, ASTNode** tree, char* func_name){
	*tree = ASTNode_new(FUNC_CALL);
	if (*tree == NULL){
		free(func_name);
		return ERR_INTERNAL;
	}
	
	// assign func name
	(*tree)->a = (void*)func_name;

	// we cannot start from the identifier as it was unreversibly consumed before
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
		return ERR_SYNTAX;
	
	ERR = ll_func_call_args(buffer_string, (ASTNode**)(&((*tree)->b)));
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
	// save our position at the start of the firts arg just in case
	ERR = save_current_file_offset();
	if (ERR)
		return ERR;

	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_PARENTHESIS_RIGHT){ // no args
		unget_token();
		return OK;
	}

	// go back before the fist arg
	// Using this function because ll_func_call_arg() has to look 2 token ahead and has to have its file offset set before the 1st arg
	ERR = rollback_to_saved_file_offset();
	if (ERR)
		return ERR;

	do{ // Look mom! I am using a do-while loop!
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
	*tree = ASTNode_new(FUNC_CALL_ARG);
	if (*tree == NULL)
		return ERR_INTERNAL;

	ERR = save_current_file_offset(); // save our position at the start of the current arg
	if (ERR)
		return ERR;

	// call GET_TOKEN() 2x because we want to look two tokens ahead to decide how to process the arg
	GET_TOKEN(true);
	GET_TOKEN(true);

	// immediately go back to the first token of the arg so the following functions can work normally
	ERR = rollback_to_saved_file_offset();
	if (ERR)
		return ERR;

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

	ERR = precedent(buffer_string, (exp_node**)(&(tree->b)), false);
	if (ERR)
		return ERR;
	
	return OK;
}

Error ll_func_call_arg_without_name(BufferString* buffer_string, ASTNode* tree){
	tree->a = NULL; // the argument was used without a name

	ERR = precedent(buffer_string, (exp_node**)(&(tree->b)) , false);
	if (ERR)
		return ERR;
	
	return OK;
}


Error ll_assign(BufferString* buffer_string, ASTNode** tree, char* var_name){
	*tree = ASTNode_new(ASSIGN);
	if (*tree == NULL){
		free(var_name);
		return ERR_INTERNAL;
	}

	(*tree)->a = var_name;

	// we cannot start from identifier
	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_ASSIGN)
		return ERR_SYNTAX;
	
	return precedent(buffer_string, (exp_node**)(&((*tree)->b)), false);
}

// var id: type = exp
// var id: type
// var id = exp
Error ll_let_var_declaration(BufferString* buffer_string, ASTNode* tree){
	// create subtrees
	tree->a = (void*)ASTNode_new(VAR_TYPE);
	if (tree->a == NULL)
		return ERR_INTERNAL;
	tree->b = (void*)ASTNode_new(VAR_HEAD);
	if (tree->b == NULL)
		return ERR_INTERNAL;

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;

	// assign name
	((ASTNode*)(tree->b))->a = (void*)BufferString_get_as_string(buffer_string);
	if (((ASTNode*)(tree->b))->a == NULL)
		return ERR_INTERNAL;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_ASSIGN){
		((ASTNode*)(tree->a))->a = (void*)TYPE_NIL; // assign type
		((ASTNode*)(tree->a))->b = (void*)false; // assign nilable
		// assign expression
		return precedent(buffer_string, (exp_node**)(&((ASTNode*)(tree->b))->b), false);
	} else if (CURRENT_TOKEN != TOKEN_COLON) {
		return ERR_SYNTAX;
	}	

	// assign type
	GET_TOKEN(true);
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

	if (CURRENT_TOKEN != TOKEN_ASSIGN){
		// declaration without an expression
		unget_token();
		return OK;
	}
	
	// assign expression
	return precedent(buffer_string, (exp_node**)(&((ASTNode*)(tree->b))->b), false);
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

	// GET_TOKEN(true);
	// if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
	// 	return ERR_SYNTAX;
	
	ERR = precedent(buffer_string, (exp_node**)(&((*tree)->a)), false);
	if (ERR)
		return ERR;

	// GET_TOKEN(true);
	// if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
	// 	return ERR_SYNTAX;

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
	
	return precedent(buffer_string, (exp_node**)(&((*tree)->a)), true);
}

Error ll_if(BufferString* buffer_string, ASTNode** tree){
	// put it in a helper variable so that we don't need to use that casting hell
	ASTNode* bodies = ASTNode_new(IFELSE_BODIES);
	if (bodies == NULL)
		return ERR_INTERNAL;	

	GET_TOKEN(true);
	if (CURRENT_TOKEN != TOKEN_KEYWORD_IF)
		return ERR_SYNTAX;
	
	// GET_TOKEN(true);
	// if (CURRENT_TOKEN != TOKEN_PARENTHESIS_LEFT)
	// 	return ERR_SYNTAX;
	
	GET_TOKEN(true);
	if (CURRENT_TOKEN == TOKEN_KEYWORD_LET){
		*tree = ASTNode_new(CHECK_IF_LET);
		if (*tree == NULL)
			return ERR_INTERNAL;
		(*tree)->b = (void*)bodies;

		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_IDENTIFIER)
			return ERR_SYNTAX;
		
		char* id = BufferString_get_as_string(buffer_string);
		if (id == NULL)
			return ERR_INTERNAL;
		
		(*tree)->a = (void*)id;
		
		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_BRACE_LEFT)
			return ERR_SYNTAX;

		ERR = ll_statements(buffer_string, (ASTNode**)(&(bodies->a)));
		if (ERR)
			return ERR;

		GET_TOKEN(true);
		if (CURRENT_TOKEN != TOKEN_BRACE_RIGHT)
			return ERR_SYNTAX;

	} else { // normal if
		*tree = ASTNode_new(IFELSE);
		if (*tree == NULL)
			return ERR_INTERNAL;
		(*tree)->b = (void*)bodies;

		unget_token();
		ERR = precedent(buffer_string, (exp_node**)(&((*tree)->a)), false);
		if (ERR)
			return ERR;

		// GET_TOKEN(true);
		// if (CURRENT_TOKEN != TOKEN_PARENTHESIS_RIGHT)
		// 	return ERR_SYNTAX;

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
