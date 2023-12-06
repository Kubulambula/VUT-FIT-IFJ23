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


#define FOOTER \
	"\n\nEXIT		GF@$exitCode\n"


#define CONCAT \
	"\n# === concat ==="\
	"\nCREATEFRAME"\
	"\nDEFVAR		TF@$temp1"\
	"\nDEFVAR		TF@$temp2"\
	"\nPOPS		TF@$temp2"\
	"\nPOPS		TF@$temp1"\
	"\nCONCAT 		TF@$temp1 TF@$temp1 TF@$temp2"\
	"\nPUSHS		TF@$temp1"\
	"\n# === concat end ==="\


#define COALESTING \
	"\nCREATEFRAME"\
	"\nDEFVAR		TF@$temp"\
	"\nDEFVAR		TF@$garbage"\
	"\nPOPS		TF@$temp"\
	"\nPUSHS		TF@$temp # now we have 1st operand an stack AND in TF@$temp"\
	"\n# Compare 1st operand with nil"\
	"\nPUSHS		nil@nil"\
	"\nEQS"\
	"\nPUSHS		bool@true"\
	"\nJUMPIFEQS	coalesting_return_%d # If 1st operand is nil, return (on stack is only 2nd operand)"\
	"\nPOPS		TF@$garbage # POP the 2nd operand to garbage"\
	"\nPUSHS		TF@$temp # Push the copy of the 1st operand"\
	"\nLABEL		coalesting_return_%d"\
	"\n# === coalesting end ==="


#define IF_ELSE_START \
	"\nPUSHS			bool@true"\
	"\nJUMPIFEQS		if_else_true_branch_%d"\
	"\n# == false branch (else) =="

#define IF_ELSE_MIDDLE \
	"\nJUMP			if_else_end_%d"\
	"\n# == true branch (if) =="\
	"\nLABEL			if_else_true_branch_%d"

#define IF_ELSE_END \
	"\nLABEL			if_else_end_%d"\
	"\n# ===== IF ELSE end ====="


#define IF_LET_START \
	"\n# ===== IF LET start ====="\
	"\nPUSHS		%s@%s"\
	"\nPUSHS		nil@nil"\
	"\nJUMPIFNEQS		if_let_true_branch_%d"\
	"\n# == false branch (is nil) =="

#define IF_LET_MIDDLE \
	"\nJUMP		if_let_end_%d"\
	"\n# == true branch (is not nil) =="\
	"\nLABEL		if_let_true_branch_%d"\

#define IF_LET_END \
	"\nLABEL		if_let_end_%d"\
	"\n# ===== if_let_end ====="


#define WHILE_START \
	"\n# ===== WHILE start ====="\
	"\nLABEL		while_start_%d"

#define WHILE_MIDDLE \
	"\nPUSHS		bool@false"\
	"\nJUMPIFEQS	while_end_%d"

#define WHILE_END \
	"\nJUMP			while_start_%d"\
	"\nLABEL		while_end_%d"\
	"\n# ===== WHILE end ====="


#endif