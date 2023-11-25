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

#ifdef _WIN64
    #define PATH "C:\\Users\\jenik\\Documents\\GitHub\\VUT-FIT-IFJ23\\src\\prog"
#endif
#ifdef __linux__
    #define PATH "prog"
#endif

int main(){
    BufferString buff;
    FILE* file = fopen(PATH, "r");
    if(file == NULL){
        printf("file not open\n");
        return 1;
    }
    initLexer(file);
    buffer_string_init(&buff);

    CURRENT_TOKEN = get_token(&buff, true);
    Error err = precedent(&buff);
    if(err){
        printf("ERROR FOUND %d\n", err);
        return 1;
    }
    printf("SYNTAX CORRECT\n");
    return 0;
}