#ifndef PRE_DEFINED_H
#define PRE_DEFINED_H


#include "error.h"
#include "generator_built_in.h"


#define HEADER \
	".IFJcode23"\
	"\n# Generated with ifj23compiler by:"\
	"\n#   Jakub Jansta - xjanst02"\
	"\n#   Jan Juracka - xjurac07"\
	"\n#   Petr Necas - xnecas31"\
	"\n#   Jakub Slanina - xslani12"\
	"\n"\
	"\nDEFVAR		GF@$exitCode # Exit code for the program"\
	"\nMOVE		GF@$exitCode int@0"\
	"\n"\
	"\nJUMP		$main # Jump behind all those function definitions"\
	"\n"


#define MAIN \
	"\n\n\n# ========== Main body of the program =========="\
	"\nLABEL		$main\n"


// #define FUNC_DEFINITIONS
// 	"\n# Built-in function definitnions:"
// 	FUNCTIONS_BUILT_IN
// 	"\n# User function definitions:"
// 	Err err = generate_user_functions();
// 	if (err)
// 		return err;

#define FOOTER \
	"\n\nEXIT		GF@$exitCode\n"\


#define COALESTING \
	"\n# === coalesting ==="\
	"\nCREATEFRAME"\
	"\nDEFVAR     TF@$temp"\
	"\nPOPS      TF@$temp"\
	"\nPUSHS        TF@$temp # now we have 1st operand an stack AND in TF@$temp"\
	"\n# Compare 1st operand with nil"\
	"\nPUSHS	nil@nil"\
	"\nEQS"\
	"\nPUSHS	bool@true"\
	"\n# If 1st operand is nil, return (on stack is only 2nd operand)"\
	"\nJUMPIFEQS	coalesting_return_%d"\
	"\nCLEARS # Clear the 2nd operand from stack"\
	"\nPUSHS	TF@$temp # Push the copy of the 1st operand"\
	"\nLABEL	coalesting_return_%d"\
	"\n# === coalesting end ===\n"

#endif