#include <stdio.h>
#include "error.h"
#include "lexer.h"
#include "buffer_string.h"

int main(void) {
	BufferString b;
	BufferString_init(&b);

	initLexer(stdin);
	Token t = TOKEN_EOL;
	while(t != TOKEN_EOF){
		t = get_next_token(&b);
		print_token_as_string(t);
		if (t == TOKEN_IDENTIFIER){
			printf("Indetifier >>> %s <<<\n", b.string);
		}
		if (t == TOKEN_LITERAL_STRING){
			printf("String: \"%s\"\n", b.string);
		}
		if (t == TOKEN_ERR_LEXICAL)
			return 1;
	}



	return OK;
}