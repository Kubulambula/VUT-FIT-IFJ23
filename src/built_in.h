#ifndef LEXER_H
#define LEXER_H


/*
	Specialni promenne pouze pro IFJ23code MUSI zacinat $ (aby to nebyl nahodou stejny identifikator jako definoval uzivatel)
	Funkce predpokladaji existujici ramec LF, kde jsou definovane promenne s argumenty
	Volajici je zodpovedny za vytvoreni a uklizeni LF pro volanou funkci vcetne definice vsech argumentu pro volanou funkci
	Funkce vraci svoje vysledky na stack (instrukce PUSHS), aby se s vylesdkem dali delat operace v pripade, ze je soucasti vyrazu
	Volajici je zodpovedny za vracenou hodnotu na stacku po volani funkce
	Funkce si muze uvnitr sebe definovat kolik ramcu chce, ale musi je vsechny uklidit pred jejim ukoncenim
*/

// ----- READ FUNCTIONS -----
#define FUNC_readString \
	"\n# Function readString()->String? (build-in)"\
	"\nLABEL readString"\
	"\nDEFVAR LF@valString"\
	"\nREAD LF@valString string"\
	"\nPUSHS LF@valString"\
	"\nRETURN"

#define FUNC_readInt \
	"\n# Function readInt()->Int? (build-in)"\
	"\nLABEL readInt"\
	"\nDEFVAR LF@valInt"\
	"\nREAD LF@valInt int"\
	"\nPUSHS LF@valInt"\
	"\nRETURN"

#define FUNC_readDouble \
	"\n# Function readDouble()->Double? (build-in)"\
	"\nLABEL readDouble"\
	"\nDEFVAR LF@valDouble"\
	"\nREAD LF@valDouble float"\
	"\nPUSHS LF@valDouble"\
	"\nRETURN"

// ----- WRITE FUNCTION -----

#define FUNC_write \
	"\n# Function write(...) (built-in)"\
	"\nLABEL write"\
	"\nRETURN"

// ----- CONVERSION FUNCTIONS ------
#define FUNC_Int2Double \
	"\n# Function Int2Double(_ term: Int)->Double (build-in)"\
	"\nLABEL Int2Double"\
	"\nDEFVAR LF@valDouble"\
	"\nINT2FLOAT LF@valDouble LF@term"\
	"\nPUSHS LF@valDouble"\
	"\nRETURN"

#define FUNC_Double2Int \
	"\n# Function Double2Int(_ term: Double)->Int (build-in)"\
	"\nLABEL Double2Int"\
	"\nDEFVAR LF@valInt"\
	"\nFLOAT2INT LF@valInt LF@term"\
	"\nPUSHS LF@valInt"\
	"\nRETURN"

// ----- STRING FUNCTIONS -----
#define FUNC_length \
	"\n# Function length(_ s: String)->Int (build-in)"\
	"\nLABEL length"\
	"\nDEFVAR LF@len"\
	"\nSTRLEN LF@len LF@s"\
	"\nPUSHS LF@len"\
	"\nRETURN"

#define FUNC_substring \
	"\n# Function substring(of s: String, startingAt i: Int, endingBefore j: Int)->String?"\
	"\nLABEL substring"\
	"\n# Check for initial conditions to determine if we should return nil"\
	"\nDEFVAR LF@condition"\
	"\nLT LF@condition LF@i int@0 # Check if i < 0"\
	"\nJUMPIFEQ substring_return_nil LF@condition bool@true"\
	"\nLT LF@condition LF@j int@0 # Check if j < 0"\
	"\nJUMPIFEQ substring_return_nil LF@condition bool@true"\
	"\nGT LF@condition LF@i LF@j # Check if i > j"\
	"\nJUMPIFEQ substring_return_nil LF@condition bool@true"\
	"\nDEFVAR LF@len"\
	"\nSTRLEN LF@len LF@s # don't call length() for simplicity"\
	"\nLT LF@condition LF@i LF@len # Check if i < len"\
	"\nJUMPIFEQ substring_return_nil LF@condition bool@false # Invert the condition to i >= len"\
	"\nGT LF@condition LF@j LF@len # Check if j > len"\
	"\nJUMPIFEQ substring_return_nil LF@condition bool@true"\
	"\n# Initial conditions passed. Now run the substring"\
	"\n"\
	"\nLABEL substring_return_nil # Bad arguments for substring() - return nil"\
	"\nPUSHS nil@nil"\
	"\nRETURN"\



#endif