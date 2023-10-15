#ifndef SYNTAX_H
#define SYNTAX_H

#include "error.h"
#include "symtable.h"
#include "syntax_ll_grammar.h"



Error parse(SymTable* sym_table);


#endif