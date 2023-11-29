#include <stdio.h>
#include <stdlib.h>

#include "buffer_string.h"
#include "buffer_string.c"

#include "lexer.h"
#include "lexer.c"

//#include "syntax_ll_grammar.h"
//#include "syntax_ll_grammar.c"

#include "syntax_precedent.h"
#include "syntax_precedent.c"

//#include "ast.h"

#ifdef _WIN64
    #define PATH "C:\\Users\\jenik\\Documents\\GitHub\\VUT-FIT-IFJ23\\src\\prog"
#endif
#ifdef __linux__
    #define PATH "prog"
#endif

void print_tree(exp_node *node, int offset){
    if(node != NULL){
        for(int i = 0; i < offset; i++){
            printf("|\t");
        }
        print_token_as_string(node->type);
        if(node->type == TOKEN_IDENTIFIER || node->type == TOKEN_LITERAL_STRING){
            printf(" %s\n", node->value.s);
        }
        else if(node->type == TOKEN_LITERAL_INT){
            printf(" %d\n", node->value.i);
        }
        else if(node->type == TOKEN_LITERAL_DOUBLE){
            printf(" %f\n", node->value.d);
        }
        else{
            printf("\n");
        }
        print_tree((exp_node*)node->left, offset + 1);
        print_tree((exp_node*)node->right, offset + 1);
    }
}

int main(){
    BufferString buff;
    exp_node *node;
    FILE* file = fopen(PATH, "r");
    if(file == NULL){
        printf("file not open\n");
        return 1;
    }
    initLexer(file);
    BufferString_init(&buff);

    CURRENT_TOKEN = get_token(&buff, true);
    Error err = precedent(&node, &buff);
    if(err){
        printf("ERROR FOUND %d\n", err);
        return 1;
    }
    printf("SYNTAX CORRECT\n");
    print_tree(node, 0);
    return 0;
}