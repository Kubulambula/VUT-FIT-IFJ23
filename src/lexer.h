#ifndef LEXER_H
#define LEXER_H

// gets next char
char get_next_char();

// sets source_file
void initLexer(FILE* file);

//skips white space
void skip_white_space(char whiteSpace)

// skips single line comment
// TODO should still return '\n' as token
void skip_comments_SL();

// skips multiline comments
void skip_comments_ML();

// checks if loaded buffer_string is keyword 
bool is_keyword(BufferString* buffer_string, Token* TokenType);

// finds next token
// returns Token type
Token get_next_token(BufferString* buffer_string);

#endif