#include <stdio.h>
#include <stdlib.h>

#include "buffer_string.h"
#include "buffer_string.c"

#include "lexer.h"
#include "lexer.c"

#include "syntax_ll_grammar.h"
#include "syntax_ll_grammar.c"

#include "syntax_precedent.h"
#include "syntax_precedent.c"


int main(){
    BufferString buff;
    FILE* file = fopen("C:\\Users\\jenik\\Documents\\GitHub\\VUT-FIT-IFJ23\\src\\prog", "r");
    //FILE* file = fopen("prog", "r");
    if(file == NULL){
        printf("file not open\n");
        return 1;
    }
    initLexer(file);
    buffer_string_init(&buff);

    CURRENT_TOKEN = get_next_token(&buff);
    if(ll_program(&buff)){
        printf("SYNTAX ERROR FOUND\n");
        return 1;
    }
    printf("SYNTAX CORRECT\n");
    return 0;
}