#ifndef PRE_DEFINED_H
#define PRE_DEFINED_H


#include "error.h"
#include "generator_built_in.h"


#define HEADER \
	"\n.IFJcode23"\
	"\n# Generated with ifj23compiler by:"\
	"\n    Jakub Jansta - xjanst02"\
	"\n    Jan Juracka - xjurac07"\
	"\n    Petr Necas - xnecas31"\
	"\n    Jakub Slanina - xslani12"\
	"\n"\
	"\nDEFVAR		GF@$exitCode # Exit code for the program"\
	"\nMOVE			GF@exitCode int@0"\
	"\n"\
	"\nJUMP			main # Jump behind all those function definitions"\
	"\n"

// #define FUNC_DEFINITIONS \
// 	"\n# Built-in function definitnions:"\
// 	FUNCTIONS_BUILT_IN\
// 	"\n# User function definitions:"\
// 	Err err = generate_user_functions();\
// 	if (err)\
// 		return err;

#define FOOTER \
	"\nEXIT			GF@exitCode\n"\


// Error generate_user_functions();


#endif