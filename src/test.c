#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "lexer.c"

#include "syntax_ll_grammar.h"
#include "syntax_ll_grammar.c"


int main(){
    BufferString buff;
    FILE* file = fopen("C:\\Users\\jenik\\Documents\\GitHub\\VUT-FIT-IFJ23\\src\\prog", "r");
    if(file == NULL){
        printf("file not open\n");
        return 1;
    }
    initLexer(file);
    buffer_string_init(&buff);

    current_token = get_next_token(&buff);
    if(ll_program(&buff)){
        printf("SYNTAX ERROR FOUND\n");
        return 1;
    }
    printf("SYNTAX CORRECT\n");
    return 0;
}