#ifndef BUILT_IN_H
#define BUILT_IN_H


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
	"\n# Function write(_ argCnt: Int) (built-in)"\
	"\nLABEL write"\
	"\nLABEL writeLoop"\
	"\nJUMPIFEQ writeLoopBreak LF@argCnt int@0"\
	"\nSUB LF@argCnt LF@argCnt int@1"\
	"\nCALL $write_helper"\
	"\nJUMP writeLoop"\
	"\nLABEL writeLoopBreak"\
	"\nRETURN"

#define FUNC_$write_helper \
	"\n# Function $write_helper (built-in) - this function cannot be called by the user and is only inteded to be called by write()"\
	"\nLABEL $write_helper"\
	"\nCREATEFRAME"\
	"\nDEFVAR TF@arg"\
	"\nPOPS TF@arg"\
	"\nWRITE TF@arg"\
	"\nRETURN"\

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
	"\nDEFVAR LF@substr # Variable with the substring to be returned"\
	"\nDEFVAR LF@c # Helper variable with current char"\
	"\nLABEL substringWhile"\
	"\nJUMPIFEQ substringWhileBreak LF@i LF@j # if i == j goto substringWhileBreak"\
	"\nGETCHAR LF@c LF@s LF@i # c = s[i]"\
	"\nADD LF@i LF@i int@1 # i++"\
	"\nCONCAT LF@substr LF@substr LF@c # substr = substr + c"\
	"\nJUMP substringWhile"\
	"\nLABEL substringWhileBreak"\
	"\nPUSHS LF@substr"\
	"\nRETURN"\
	"\nLABEL substring_return_nil # Bad arguments for substring() - return nil"\
	"\nPUSHS nil@nil"\
	"\nRETURN"

#define FUNC_ord \
	"\n# Function ord(_ c: String)->Int"\
	"\nLABEL ord"\
	"\nDEFVAR LF@len"\
	"\nSTRLEN LF@len LF@c"\
	"\nJUMPIFQE ord_return_zero LF@len int@0"\
	"\nDEFVAR LF@ordVal"\
	"\nSTRI2INT LF@ordVal LF@c int@0"\
	"\nPUSHS LF@ordVal"\
	"\nRETURN"\
	"\nLABEL ord_return_zero"\
	"\nPUSHS int@0"\
	"\nRETURN"

#define FUNC_chr \
	"\n# Function chr(_ i: Int)->String"\
	"\nDEFVAR LF@chrVal"\
	"\nINT2CHAR LF@chrVal LF@i"\
	"\nPUSHS LF@chrVal"\
	"\nRETURN"\

#endif