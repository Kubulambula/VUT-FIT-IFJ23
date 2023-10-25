#include <stdio.h>
#include "lexer.h"
#include "lexer.c"
#include "buffer_string.c"

int main(){
    BufferString buff;
    Token current_token;
    FILE* file = fopen("prog", "rw");
    if(file == NULL){
        printf("file not open\n");
        return 1;
    }
    initLexer(file);
    buffer_string_init(&buff);

    current_token = get_next_token(&buff);
    while(current_token != TOKEN_EOF){
        printf("%d\n", current_token);
        current_token = get_next_token(&buff);
    }
    return 0;
}