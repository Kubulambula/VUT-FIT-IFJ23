#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "buffer_string.h"


bool buffer_string_init(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);
	
	buffer_string_p->string = malloc(sizeof(char) * BUFFER_STRING_LENGTH_MINIMAL);
	if (buffer_string_p->string == NULL){
		return false;
	}

	buffer_string_p->string[0] = '\0';
	buffer_string_p->alloc_length = BUFFER_STRING_LENGTH_MINIMAL;
	buffer_string_p->length = 0;
	return true;
}


bool buffer_string_init_from(BufferString* buffer_string_p, char* str){
	assert(buffer_string_p != NULL);
	assert(str != NULL);
	
	unsigned str_length = strlen(str);
	unsigned required_alloc_length = str_length + 1;
	if (required_alloc_length > BUFFER_STRING_LENGTH_MINIMAL){
		// make the required_alloc_length_after_append the closest multiple of BUFFER_STRING_LENGTH_INCREMENT
		required_alloc_length += BUFFER_STRING_LENGTH_INCREMENT - (required_alloc_length % BUFFER_STRING_LENGTH_INCREMENT);
	} else {
		required_alloc_length = BUFFER_STRING_LENGTH_MINIMAL;
	}

	char* new_string = malloc(sizeof(char) * required_alloc_length);
	if (new_string == NULL){
		return false;
	}

	memcpy(buffer_string_p->string, str, sizeof(char) * (str_length + 1)); // don't forget to copy the null terminator
	buffer_string_p->alloc_length = required_alloc_length;
	buffer_string_p->length = str_length;
	return true;
}


void buffer_string_free(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);

	if (buffer_string_p->string != NULL){
		free(buffer_string_p->string);
	}
	buffer_string_p->string = NULL;
	buffer_string_p->alloc_length = 0;
	buffer_string_p->length = 0;
}


void buffer_string_clear(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);

	if (buffer_string_p->string != NULL){
		buffer_string_p->string[0] = '\0';
	}
	buffer_string_p->length = 0;
}


char* buffer_string_to_string(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);

	char* str = malloc(sizeof(char) * (buffer_string_p->length + 1));
	if (str != NULL){
		memcpy(str, buffer_string_p->string, sizeof(char) * (buffer_string_p->length + 1));
	}
	return str;
}


bool buffer_string_append_char(BufferString* buffer_string_p, char c){
	assert(buffer_string_p != NULL);
	assert(buffer_string_p->string != NULL);

	if (buffer_string_p->length + 1 == buffer_string_p->alloc_length){
		char* new_string = realloc(buffer_string_p->string, sizeof(char) * (buffer_string_p->alloc_length + BUFFER_STRING_LENGTH_INCREMENT));
		if (new_string == NULL){
			return false;
		}
		buffer_string_p->string = new_string;
		buffer_string_p->alloc_length += BUFFER_STRING_LENGTH_INCREMENT; 
	}

	buffer_string_p->string[(buffer_string_p->length)++] = c;
	buffer_string_p->string[buffer_string_p->length] = '\0';
	return true;
}


bool buffer_string_append_str(BufferString* buffer_string_p, char* str){
	assert(buffer_string_p != NULL);
	assert(str != NULL);
	
	unsigned str_length = strlen(str);
	unsigned required_alloc_length_after_append = buffer_string_p->length + str_length + 1;
	if (required_alloc_length_after_append > buffer_string_p->alloc_length){
		// make the required_alloc_length_after_append the closest multiple of BUFFER_STRING_LENGTH_INCREMENT
		required_alloc_length_after_append += BUFFER_STRING_LENGTH_INCREMENT - (required_alloc_length_after_append % BUFFER_STRING_LENGTH_INCREMENT);
		char* new_string = realloc(buffer_string_p->string, sizeof(char) * required_alloc_length_after_append);
		if (new_string == NULL){
			return false;
		}
		buffer_string_p->string = new_string;
		buffer_string_p->alloc_length = required_alloc_length_after_append;
	}

	strcat(buffer_string_p->string, str);
	buffer_string_p->length += str_length;
	return true;
}


bool buffer_string_cmp_str(BufferString* buffer_string_p, char* str){
	assert(buffer_string_p != NULL);
	assert(str != NULL);
	return strcmp(buffer_string_p->string, str);
}


unsigned buffer_string_get_length(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);
	return buffer_string_p->length;
}


// Clamps long range to int range just in case the architecture has different sizes for int and long
static int clamp_long_to_int(long l){
	if (l < INT_MIN)
		return INT_MIN;
	if (l > INT_MAX)
		return INT_MAX;
	return l;
}


int buffer_string_escape(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);
	// Setup temp BufferString
	BufferString temp;
	if(buffer_string_init(&temp))
		return true;


	// Copy temp to buffer_string_p
	free(buffer_string_p->string);
	buffer_string_p->alloc_length = temp.alloc_length;
	buffer_string_p->length = temp.length;
	buffer_string_p->string = temp.string;
	// DO NOT FREE temp!!! It's on the stack so it is freed after return and the string is passed to buffer_string_p
	return true;
}


int buffer_string_get_as_int(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);
#ifdef NDEBUG
	// Just return without any checks if not debug - returns 0 on error
	return clamp_long_to_int(strtol(buffer_string_p->string, NULL, 10));
#else
	char* char_after_value;
	int value = clamp_long_to_int(strtol(buffer_string_p->string, &char_after_value, 10));
	// If char_after_value equals buffer_string_p->string, then we know that there was nothing converted - no numerical value on the beginning of string.
	assert(char_after_value != buffer_string_p->string); 
	return value;
#endif
}


double buffer_string_get_as_double(BufferString* buffer_string_p){
	assert(buffer_string_p != NULL);
#ifdef NDEBUG
	// Just return without any checks if not debug - returns 0.0 on error
	return strtod(buffer_string_p->string, NULL);
#else
	char* char_after_value;
	double value = strtod(buffer_string_p->string, &char_after_value);
	// If char_after_value equals buffer_string_p->string, then we know that there was nothing converted - no numerical value on the beginning of string.
	assert(char_after_value != buffer_string_p->string); 
	return value;
#endif
}
