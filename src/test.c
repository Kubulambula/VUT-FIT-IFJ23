#include <stdio.h>
#include <errno.h>

#include "error.h"
#include "lexer.h"
#include "lexer.c"
#include "buffer_string.h"
#include "buffer_string.c"
#include "syntax_ll.h"
#include "syntax_ll.c"
#include "ast.h"
#include "ast.c"
#include "syntax_precedent.h"
#include "syntax_precedent.c"

#ifdef _WIN64
    #define PATH "C:\\Users\\jenik\\Documents\\GitHub\\VUT-FIT-IFJ23\\src\\prog"
#endif
#ifdef __linux__
    #define PATH "prog"
#endif

int main(void) {
    FILE *file = fopen(PATH, "r");
    if(file == NULL){
        printf("File not open\n");
        return 0;
    }
	initLexer(file);

	BufferString buffer_string;
	if (!BufferString_init(&buffer_string))
		return ERR_INTERNAL;

	ASTNode* ast = ASTNode_new(ROOT);
	if (ast == NULL)
		return ERR_INTERNAL;

	Error err = ll_program(&buffer_string, &ast);
	printf("ERR: %d\n", err);
	return err;
}