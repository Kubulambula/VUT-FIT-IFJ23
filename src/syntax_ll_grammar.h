#ifndef SYNTAX_LL_GRAMMAR_H
#define SYNTAX_LL_GRAMMAR_H

#include <stdio.h>
#include <stdbool.h>
#include "buffer_string.h"
#include "error.h"

Error ll_while(BufferString* buffer_string);

Error ll_while_head(BufferString* buffer_string);

Error ll_if(BufferString* buffer_string);

Error ll_if_head(BufferString* buffer_string);

Error ll_func_definition(BufferString* buffer_string);

Error ll_func_head(BufferString* buffer_string);

Error ll_func_params(BufferString* buffer_string);

Error ll_func_param(BufferString* buffer_string);

Error ll_func_more_params(BufferString* buffer_string);

Error ll_func_type(BufferString* buffer_string);

Error ll_statement_body(BufferString* buffer_string);

Error ll_program(BufferString* buffer_string);



Error ll_func_call(BufferString* buffer_string);

Error ll_func_args(BufferString* buffer_string);

Error ll_func_arg(BufferString* buffer_string);

Error ll_func_more_arg(BufferString* buffer_string);

Error ll_type(BufferString* buffer_string);

Error ll_lit(BufferString* buffer_string);

Error ll_lit(BufferString* buffer_string);

Error ll_val(BufferString* buffer_string);

Error ll_more_val(BufferString* buffer_string);

Error ll_exp(BufferString* buffer_string);

Error ll_more_exp(BufferString* buffer_string);

Error ll_expressions(BufferString* buffer_string);

Error ll_statement(BufferString* buffer_string);

Error ll_statements(BufferString* buffer_string);
#endif