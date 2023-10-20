#include <stdio.h>
#include "lexer.h"
#include "syntax_ll_grammar.h"
#include "syntax_ll_grammar.c"


int main(){
    BufferString buff;
    FILE* file = fopen("program", "r");
    initLexer(file);
    buffer_string_init(&buff);

    current_token = get_next_token(&buff);
    current_token;
    if(ll_expressions(&buff)){
        printf("good job\n");
        return 0;
    }
    return 1;
}