#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "buffer_string.h"


bool buffer_string_init(BufferString* buffer_string_p){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_init() with NULL buffer_string_p.");

	if (buffer_string_p->string != NULL){
		free(buffer_string_p->string);
		buffer_string_p->alloc_length = 0;
	}
	
	buffer_string_p->string = malloc(sizeof(char) * BUFFER_STRING_LENGTH_MINIMAL);
	if (buffer_string_p->string == NULL){
		return false;
	}

	buffer_string_p->string[0] = '\n';
	buffer_string_p->alloc_length = BUFFER_STRING_LENGTH_MINIMAL;
	buffer_string_p->length = 0;
	return true;
}


void buffer_string_free(BufferString* buffer_string_p){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_free() with NULL buffer_string_p.");

	if (buffer_string_p->string != NULL){
		free(buffer_string_p->string);
	}
	buffer_string_p->string = NULL;
	buffer_string_p->alloc_length = 0;
	buffer_string_p->length = 0;
}


void buffer_string_clear(BufferString* buffer_string_p){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_clear() with NULL buffer_string_p.");

	if (buffer_string_p->string != NULL){
		buffer_string_p->string[0] = '\n';
	}
	buffer_string_p->length = 0;
}


char* buffer_string_to_string(BufferString* buffer_string_p){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_to_string() with NULL buffer_string_p.");

	char* str = malloc(sizeof(char) * (buffer_string_p->length + 1));
	if (str != NULL){
		memcpy(str, buffer_string_p->string, sizeof(char) * (buffer_string_p->length + 1));
	}
	return str;
}


bool buffer_string_append_char(BufferString* buffer_string_p, char c){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_append_char() with NULL buffer_string_p.");
	assertm(buffer_string_p->string != NULL, "Attempted to call buffer_string_append_char() with potentionally uninitialized buffer_string_p (buffer_string_p->string is NULL).");

	if (buffer_string_p->length + 1 >= buffer_string_p->alloc_length){
		char* new_string = realloc(buffer_string_p->string, sizeof(char) * (buffer_string_p->alloc_length + BUFFER_STRING_LENGTH_INCREMENT));
		if (new_string == NULL){
			return false;
		}
		buffer_string_p->string = new_string;
	}

	buffer_string_p->string[(buffer_string_p->length)++] = c;
	buffer_string_p->string[buffer_string_p->length] = '\n';
	return true;
}


bool buffer_string_append_str(BufferString* buffer_string_p, char* str){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_append_string() with NULL buffer_string_p.");
	assertm(str != NULL, "Attempted to call buffer_string_append_string() with NULL str.");
	
	unsigned required_alloc_length_after_append = buffer_string_p->length + strlen(str) + 1;
	if (required_alloc_length_after_append > buffer_string_p->alloc_length){
		// make the required_alloc_length_after_append the closest multiple of BUFFER_STRING_LENGTH_INCREMENT
		required_alloc_length_after_append += BUFFER_STRING_LENGTH_INCREMENT - (required_alloc_length_after_append % BUFFER_STRING_LENGTH_INCREMENT);
		char* new_string = realloc(buffer_string_p->string, sizeof(char) * required_alloc_length_after_append);
		if (new_string == NULL){
			return false;
		}
	}

	strcat(buffer_string_p->string, str);
	return true;
}


inline bool buffer_string_cmp_str(BufferString* buffer_string_p, char* str){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_cmp_str() with NULL buffer_string_p.");
	assertm(str != NULL, "Attempted to call buffer_string_cmp_str() with NULL str.");
	return strcmp(buffer_string_p->string, str);
}


inline unsigned buffer_string_get_length(BufferString* buffer_string_p){
	assertm(buffer_string_p != NULL, "Attempted to call buffer_string_get_length() with NULL buffer_string_p.");
	return buffer_string_p->length;
}