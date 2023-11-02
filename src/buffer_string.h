#ifndef BUFFER_STRING_H
#define BUFFER_STRING_H

#include <stdbool.h>


// Minimum length of the BufferString. It will be initialized with this size.
#define BUFFER_STRING_LENGTH_MINIMAL 16
// Length increment of BufferString. Everytime the another character/string can't be appended, the alloc_lenght is incremented until it can hold the new string.
#define BUFFER_STRING_LENGTH_INCREMENT 8


typedef struct{
	char* string; // string representation of BufferString
	unsigned alloc_length; // number of allocated chars in BufferString (including the null terminator '\0')
	unsigned length; // number of chars in BufferString (not including the null terminator '\0')
} BufferString;


// Initializes BufferString with BUFFER_STRING_LENGTH_MINIMAL.
// Returns true on success and false otherwise.
bool buffer_string_init(BufferString* buffer_string_p);

// Initializes BufferString with content of str.
// Returns true on success and false otherwise.
// str MUST BE NULL TERMINATED. Otherwise the behavior is undefined.
bool buffer_string_init_from(BufferString* buffer_string_p, char* str);

// Frees and uninitializes BufferString.
void buffer_string_free(BufferString* buffer_string_p);

// Clears BufferString. After this, BufferString represents an empty string.
void buffer_string_clear(BufferString* buffer_string_p);

// Returns a string that is represented by BufferString.
// The returned string will be allocated to fit the string precisely.
char* buffer_string_to_string(BufferString* buffer_string_p);

// Appends the char c to end of the BufferString.
// If the allocated length cannot fit the new string, it will be resized and reallocated.
bool buffer_string_append_char(BufferString* buffer_string_p, char c);

// Appends the string str to end of the BufferString.
// If the allocated length cannot fit the new string, it will be resized and reallocated.
// str MUST BE NULL TERMINATED. Otherwise the behavior is undefined.
bool buffer_string_append_str(BufferString* buffer_string_p, char* str);

// Returns the result of strcmp of BufferString and string str.
bool buffer_string_cmp_str(BufferString* buffer_string_p, char* str);

// Returns the length of BufferString.
unsigned buffer_string_get_length(BufferString* buffer_string_p);

// Escapes the BufferString to the IFJ23 specifications
// Returns 1 on success and 0 when there was an error in the escape sequence. If -1 is returned, it means an internal error.
int buffer_string_escape(BufferString* buffer_string_p);

// Reads the string from left to right and tries to convert it to int.
// After encountering the first characher, that cannot be convertet, it stops and converts all the previous characters. Read the strtol() manual.
// If nothing can be converted, returns 0 on non debug builds.
int buffer_string_get_as_int(BufferString* buffer_string_p);

// Reads the string from left to right and tries to convert it to double.
// After encountering the first characher, that cannot be convertet, it stops and converts all the previous characters. Read the strtod() manual.
// If nothing can be converted, returns 0.0 on non debug builds.
double buffer_string_get_as_double(BufferString* buffer_string_p);

#endif
