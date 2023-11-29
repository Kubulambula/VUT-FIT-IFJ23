#include "syntax.h"

#include <stdbool.h>
#include "buffer_string.h"



Error parse(SymTable* sym_table){
	BufferString b;
	return ll_program(&b);
}

