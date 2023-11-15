#ifndef ERROR_H
#define ERROR_H

typedef enum{
	OK = 0, // no error
	ERR_OK = 0, // no error
	ERR_LEXICAL = 1, // lexical error
	ERR_SYNTAX = 2, // syntax error
	ERR_SEMATIC_UNDEFINED_FUNC = 3, // sematic error - undefined function called / variable redefinition
	ERR_SEMATIC_REDEFINED = 3,
	ERR_SEMATIC_BAD_FUNC_ARG_COUNT = 4, // sematic error - function called with a wrong amount of arguments
	ERR_SEMATIC_BAD_FUNC_ARG_TYPE = 4, // sematic error - function called with a wrond argument type
	ERR_SEMATIC_BAD_FUNC_RETURN_TYPE = 4, // sematic error - function return statement is not compatible with function return type
	ERR_SEMATIC_UNDEFINED_VAR = 5, // sematic error - undefined variable used
	ERR_SEMATIC_VOID_FUNC_RETURNS_VALUE = 6, // sematic error - void function does have a return statement WITH EXPRESSION (return without expression is valid)
	ERR_SEMATIC_NON_VOID_FUNC_DOESNT_RETURN_VALUE = 6, // sematic error - non-void function has missing return statement or the return statement is missing a expression
	ERR_SEMATIC_INCOMPATIBLE_TYPES = 7, // sematic error - types in expression are not compatible or type is not compatible with variable
	ERR_SEMATIC_BAD_TYPE_INFERENCE = 8, // sematic error - type cannot be inferred
	ERR_SEMATIC_OTHER = 9, // other sematic errors
	ERR_INTERNAL = 99, // internal error - e.g. failed malloc()
#ifdef NDEBUG
	ERR_DEBUG = 99, // if debug is disabled, handle it like ERR_INTERNAL
#else
	ERR_DEBUG = 100 // debug error
#endif
} Error;


#endif