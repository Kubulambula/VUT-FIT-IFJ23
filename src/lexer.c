#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "lexer.h"



FILE* source_file = NULL;

Token CURRENT_TOKEN;

Token last_token = TOKEN_ERR_INTERNAL;
bool use_last_token = false;

long saved_file_offset = -1;


void initLexer(FILE* file){
    // sanity check
    assert(file != NULL);

    if (file == NULL){
        file = stdin;
    }
    source_file = file;
}


char get_next_char(){
#if !defined NDEBUG && defined VERBOSE // only print in verbose debug
    char Nextchar = getc(source_file);
    printf("Read char: %c\n", Nextchar);
    return Nextchar;
#else
    return getc(source_file);
#endif
}


void skip_white_space(char whiteSpace){
    while(isblank(whiteSpace)) {
        whiteSpace = get_next_char();
    }
    ungetc(whiteSpace, source_file);
}


// Handle comments mutli line
// We have to handle inner multiline comments
bool skip_comments_ML(){
    int depth = 1; // we start already in side the comment
    char Char = '\0';
    char prevChar = '\0';
    do{
        prevChar = Char;
        Char = get_next_char();
        // comment block start
        if(prevChar == '/' && Char == '*'){
            depth++;
            Char = '\0'; // just so that /* /*/* */*/ */ works correctly
            continue;
        }
        // comment block end
        if(prevChar == '*' && Char == '/'){
            if (--depth == 0)
                // If we exited all the inner blocks, we know that the block comment is over
                return true;
            // We are still inside block comment
            Char = '\0'; // just so that /* /**/* */ works correctly
            continue;
        }
        // unexpected EOF inside block comment
        if(Char == EOF)
            return false;
    }while(1);
}


// Handle comments single line
void skip_comments_SL(){
    char Char;
    do{
        Char = get_next_char();
        if (Char == '\n')
            return;
        
        if (Char == EOF){
            ungetc(EOF, source_file);
            return;
        }
    }while(1);
}


// Handle keyword matching
bool is_keyword(BufferString* buffer_string, Token* TokenType){
    // Offset of keywords in Token enum.
    int offset = 4;

    // Array of IFJ23 keywords
    char *keywords[] = {
        "nil", "Int", "Double", "String", "var", "let", "if", "else",
        "while", "func", "return"
    };

    // Calculate the number of keywords
    size_t numKeywords = sizeof(keywords) / sizeof(keywords[0]);

    // Check if the input string matches any keyword
    for (size_t i = 0; i < numKeywords; ++i) {
        if (BufferString_cmp_str(buffer_string, keywords[i]) == 0) {
            *TokenType = i + offset;
            return true;  // Its a keyword
        }
    }

    return false;  // Not a keyword
}


// Helper pro escape_string()
Error escape_string_hex(BufferString* buffer_string){
    if (get_next_char() != '{')
        return ERR_LEXICAL;
    
    char hex_code[9] = {'\0'};
    for (int i=0; i<8; i++){
        hex_code[i] = get_next_char();
        // check for code shorter than 8 chars
        if (hex_code[i] == '}'){
            hex_code[i] = '\0';
            ungetc('}', source_file);
            break;
        }
        // if the char was not valid hex char (0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f,A,B,C,D,E,F), return error
        if (!isxdigit(hex_code[i]))
            return ERR_LEXICAL;
    }
    // check if the escape code was terminated
    if (get_next_char() != '}')
        return ERR_LEXICAL;
    // check if the code has at least one hexadecimal num
    if (hex_code[0] == '\0')
        return ERR_LEXICAL;
    // convert the hex num to ascii char by casting the long code to char (idk how else to handle it)
    // should we check the value for EOF, NULL or something line that???
    return BufferString_append_char(buffer_string, (char)strtol(hex_code, NULL, 16)) ? OK : TOKEN_ERR_INTERNAL;
}


// Vraci OK, kdyz uspesne do buffer_stringu zapise escaped znak, nebo korespondujici chybu
Error escape_string(BufferString* buffer_string){
    char nextChar = get_next_char();
    if (nextChar == '"'){
        if (!BufferString_append_char(buffer_string, '"'))
            return ERR_INTERNAL;
    }
    else if (nextChar == 'n'){
        if (!BufferString_append_char(buffer_string, '\n'))
            return ERR_INTERNAL;
    }
    else if (nextChar == 'r'){
        if (!BufferString_append_char(buffer_string, '\r'))
            return ERR_INTERNAL;
    }
    else if (nextChar == 't'){
        if (!BufferString_append_char(buffer_string, '\t'))
            return ERR_INTERNAL;
    }
    else if (nextChar == '\\'){
        if (!BufferString_append_char(buffer_string, '\\'))
            return ERR_INTERNAL;
    }
    else if (nextChar == 'u')
        return escape_string_hex(buffer_string);
    else
        return ERR_LEXICAL;
    return OK;
}


Error save_current_file_offset(){
    saved_file_offset = ftell(source_file);
    return (saved_file_offset != -1) ? OK : ERR_INTERNAL;
}


Error rollback_to_saved_file_offset(){
    if (saved_file_offset == -1)
        return ERR_INTERNAL;
    
    int err = fseek(source_file, saved_file_offset, SEEK_SET);
    if (err != 0)
        return ERR_INTERNAL;
    
    last_token = TOKEN_ERR_INTERNAL;
    return OK;
}


// obal pro get_next_token(). Vraci dalsi token a pokud je pred volanim pouzito unget_token(), cte stejny token znovu.
Token get_token(BufferString* buffer_string, bool skip_eol){
    if (use_last_token){
        use_last_token = false;
        return last_token;
    }
    do{
        last_token = get_next_token(buffer_string);// get next token
    } while(skip_eol && last_token == TOKEN_EOL); // go to the next token, if the token is EOL
    return last_token;
}


// Zajisti, ze prave precteny token se precte znovu - nekolikanasobne volani porad cte jeden posledni token
void unget_token(){
    use_last_token = true;
}


// Funkce najde dalsi token
Token get_next_token(BufferString* buffer_string){
        char nextChar;
        State state = LEXER_STATE_START;
        Error err;
        // Clear the BufferString from the junk accumulated from previous token
        BufferString_clear(buffer_string);

        while(1){
            nextChar = get_next_char();

            switch (state) {
                case LEXER_STATE_START:
                    //White space
                    if(isblank(nextChar)){
                        skip_white_space(nextChar);
                    }
                    
                    //Comment
                    else if(nextChar == '/'){
                        state = LEXER_STATE_POSSIBLE_COMMENT;
                    }
                    
                    //END OF FILE
                    else if(nextChar == EOF){
                        return TOKEN_EOF;
                    }

                    //END OF LINE
                    else if(nextChar == '\n'){
                        return TOKEN_EOL;
                    }

                    //token is int or double
                    else if(isdigit(nextChar)){
                        state = LEXER_STATE_NUMBER;
                        ungetc(nextChar,source_file); //unreading char to get read it again int the next state
                    }

                    //token is keyword or identifier
                    else if(isalpha(nextChar) || nextChar == '_'){
                        state = LEXER_STATE_IDENTIFIER_OR_KEYWORD;
                        ungetc(nextChar,source_file); //unreading char to get read it again int the next state
                    }    
                    
                    //token is string
                    else if(nextChar == '"'){
                        state = LEXER_STATE_STRING_BEGIN;
                    }

                    //token is plus
                    else if(nextChar == '+'){
                        return TOKEN_OPERATOR_PLUS;
                    }

                    //token is multiplication
                    else if(nextChar == '*'){
                        return TOKEN_OPERATOR_MULTIPLICATION;
                    }

                    //token is comma
                    else if(nextChar == ','){
                        return TOKEN_COMMA;
                    }

                    //token is colon
                    else if(nextChar == ':'){
                        return TOKEN_COLON;
                    }

                    //token could be minus or arrow
                    else if(nextChar == '-'){
                        state = LEXER_STATE_MINUS_OR_ARROW;
                    }

                    //token could be "less than" or "less than or equal"
                    else if(nextChar == '<'){
                        state = LEXER_STATE_LESS_POSSIBLE_EQUAL;
                    }

                    //token could be "more than" or "more than or equal"
                    else if(nextChar == '>'){
                        state = LEXER_STATE_GREATER_POSSIBLE_EQUAL;
                    }

                    //token could be "EXCLAMATION" or "not equal"
                    else if(nextChar == '!'){
                        state = LEXER_STATE_POSSIBLE_NOT_EQUALS;
                    }

                    //token could be "assign" or "equal"
                    else if(nextChar == '='){
                        state = LEXER_STATE_POSSIBLE_EQUALS;
                    }

                    //token could be "question" or "coalescing"
                    else if(nextChar == '?'){
                        state = LEXER_STATE_POSSIBLE_COALESCING;
                    }

                    //token is left parenthesis
                    else if(nextChar == '('){
                        return TOKEN_PARENTHESIS_LEFT;
                    }

                    //token is right parenthesis
                    else if(nextChar == ')'){
                        return TOKEN_PARENTHESIS_RIGHT;
                    }

                    //token is left brace
                    else if(nextChar == '{'){
                        return TOKEN_BRACE_LEFT;
                    }

                    //token is right brace
                    else if(nextChar == '}'){
                        return TOKEN_BRACE_RIGHT;
                    }

                    else {
                        //Possible for white space to fall through start state
                        //Character cannot be processed
                        return TOKEN_ERR_LEXICAL;
                    }

                    break;

                // INTERESTING STATES

                case LEXER_STATE_POSSIBLE_COMMENT:
                    if(nextChar == '*'){
                        if (!skip_comments_ML())
                            return TOKEN_ERR_LEXICAL;
                        state = LEXER_STATE_START;
                    }   
                    else if(nextChar == '/'){
                        skip_comments_SL();
                        state = LEXER_STATE_START;
                    }
                    else {
                        ungetc(nextChar,source_file);
                        return TOKEN_OPERATOR_DIVISION;
                    }
                    break;
                
                case LEXER_STATE_NUMBER:
                    if (isdigit(nextChar)){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                    } else if(nextChar == '.'){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        state = LEXER_STATE_DOUBLE_DOT;
                    } else if(nextChar == 'e' || nextChar == 'E'){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        state = LEXER_STATE_DOUBLE_E;
                    } else{
                        ungetc(nextChar, source_file);
                        return TOKEN_LITERAL_INT;
                    }
                    break;
                
                case LEXER_STATE_DOUBLE_DOT:
                    if (isdigit(nextChar)){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        state = LEXER_STATE_DOUBLE_AFTER_DOT;
                        break;
                    }
                    ungetc(nextChar, source_file);
                    return TOKEN_ERR_LEXICAL;
               
                case LEXER_STATE_DOUBLE_AFTER_DOT:
                    if (isdigit(nextChar)){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                    } else if(nextChar == 'e' || nextChar == 'E'){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        state = LEXER_STATE_DOUBLE_E;
                    } else{
                        ungetc(nextChar, source_file);
                        return TOKEN_LITERAL_DOUBLE;
                    }
                    break;
                    
                case LEXER_STATE_DOUBLE_E:
                    if (isdigit(nextChar)){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        state = LEXER_STATE_DOUBLE_AFTER_E;
                        break;
                    } else if (nextChar == '-' || nextChar == '+'){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        state = LEXER_STATE_DOUBLE_E_SIGN;
                        break;
                    }
                    ungetc(nextChar, source_file);
                    return TOKEN_ERR_LEXICAL;
                
                case LEXER_STATE_DOUBLE_E_SIGN:
                    if (isdigit(nextChar)){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        state = LEXER_STATE_DOUBLE_AFTER_E;
                        break;
                    }
                    ungetc(nextChar, source_file);
                    return TOKEN_ERR_LEXICAL;
                
                case LEXER_STATE_DOUBLE_AFTER_E:
                    if (isdigit(nextChar)){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        break;
                    }
                    ungetc(nextChar, source_file);
                    return TOKEN_LITERAL_DOUBLE;

                case LEXER_STATE_IDENTIFIER_OR_KEYWORD:
                    // This matches both alpha AND numerical, but we can get to this state only from alpha.
                    // Meaning we don't need to check if the first char is numerical. Good job Kunikus
                    if(isalnum(nextChar) || nextChar == '_'){
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                        break;
                    }
                    ungetc(nextChar,source_file); // to read unknown char again for the start state
                    Token tokenType;
                    return is_keyword(buffer_string, &tokenType) ? tokenType : TOKEN_IDENTIFIER;

                case LEXER_STATE_STRING_BEGIN:
                    if (nextChar == '"'){ // this would be the 2nd " found
                        state = LEXER_STATE_POSSIBLE_MULTILINE_STRING;
                        break;
                    }
                    // The character after first " was not "
                    ungetc(nextChar, source_file);
                    state = LEXER_STATE_STRING;
                    break;
                
                case LEXER_STATE_STRING:
                    if (nextChar == '"'){
                        return TOKEN_LITERAL_STRING;
                    } else if (nextChar == '\\'){ // string escape sequence
                        state = LEXER_STATE_STRING_ESCAPE;
                    } else if (nextChar == '\n'){
                        return TOKEN_ERR_LEXICAL;
                    } else if (nextChar == EOF){
                        return TOKEN_ERR_LEXICAL;
                    } else{
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                    }
                    break;
                
                case LEXER_STATE_STRING_ESCAPE:
                    ungetc(nextChar, source_file); // push the char back for escape_string()
                    err = escape_string(buffer_string);
                    if (err == ERR_INTERNAL)
                        return TOKEN_ERR_INTERNAL;
                    else if (err == ERR_LEXICAL)
                        return TOKEN_ERR_LEXICAL;
                    // sanity check
                    assert(err == OK);
                    // it was ok, so return back to normal string
                    state = LEXER_STATE_STRING;
                    break;
                
                case LEXER_STATE_POSSIBLE_MULTILINE_STRING:
                    if (nextChar == '"'){ // 3rd "
                        state = LEXER_STATE_MULTILINE_STRING_OPENING;
                        break;
                    }
                    return TOKEN_LITERAL_STRING; // it was just an empty string
                
                case LEXER_STATE_MULTILINE_STRING_OPENING:
                    if (nextChar == '\n'){ // EOL at the end of """
                        state = LEXER_STATE_MULTILINE_STRING;
                        break;
                    }
                    // Go back to the second " and return an empty string
                    ungetc('"', source_file);
                    ungetc('\n', source_file);
                    return TOKEN_LITERAL_STRING;
                
                case LEXER_STATE_MULTILINE_STRING:
                    if (nextChar == '\n'){
                        state = LEXER_STATE_MULTILINE_STRING_EOL;
                    } else if (nextChar == '"'){ // 1st " that is not excaped or preceded with \n 
                        state = LEXER_STATE_MULTILINE_STRING_FIRST_QUOTE;
                    } else if (nextChar == '\\'){
                        state = LEXER_STATE_MULTILINE_STRING_ESCAPE;
                    } else if (nextChar == EOF){
                        return TOKEN_ERR_LEXICAL;
                    } else {
                        if (!BufferString_append_char(buffer_string, nextChar))
                            return TOKEN_ERR_INTERNAL;
                    }
                    break;
                
                case LEXER_STATE_MULTILINE_STRING_ESCAPE:
                    ungetc(nextChar, source_file); // push the char back for escape_string()
                    err = escape_string(buffer_string);
                    if (err == ERR_INTERNAL)
                        return TOKEN_ERR_INTERNAL;
                    else if (err == ERR_LEXICAL)
                        return TOKEN_ERR_LEXICAL;
                    // sanity check
                    assert(err == OK);
                    // it was ok, so return back to normal string
                    state = LEXER_STATE_MULTILINE_STRING;
                    break;
                
                case LEXER_STATE_MULTILINE_STRING_FIRST_QUOTE:
                    if (nextChar == '"'){ // 2nd " that is not excaped or preceded with \n 
                        state = LEXER_STATE_MULTILINE_STRING_SECOND_QUOTE;
                    } else {
                        if (!BufferString_append_char(buffer_string, '"'))
                            return TOKEN_ERR_INTERNAL;
                        ungetc(nextChar, source_file);
                        state = LEXER_STATE_MULTILINE_STRING;
                    }
                    break;
                
                case LEXER_STATE_MULTILINE_STRING_SECOND_QUOTE:
                    if (nextChar == '"'){ // 3rd " that is not excaped or preceded with \n
                        // Three quotes in multiline string without preceeding EOL is forbidden
                        // This should be a lexer error IMO, but can be handled as syntax in the future
                        return TOKEN_ERR_LEXICAL;
                    }
                    if (!BufferString_append_char(buffer_string, '"'))
                        return TOKEN_ERR_INTERNAL;
                    if (!BufferString_append_char(buffer_string, '"'))
                        return TOKEN_ERR_INTERNAL;
                    ungetc(nextChar, source_file);
                    state = LEXER_STATE_MULTILINE_STRING;
                    break;
                
                case LEXER_STATE_MULTILINE_STRING_EOL:
                    if (nextChar == '"'){ // 1st " after \n
                        state = LEXER_STATE_POSSIBLE_MULTILINE_STRING_END;
                    } else if(nextChar == EOF){
                        return TOKEN_ERR_LEXICAL;
                    } else{
                        if (!BufferString_append_char(buffer_string, '\n'))
                            return TOKEN_ERR_INTERNAL;
                        ungetc(nextChar, source_file);
                        state = LEXER_STATE_MULTILINE_STRING;
                    }
                    break;
                
                case LEXER_STATE_POSSIBLE_MULTILINE_STRING_END:
                    if (nextChar == '"'){ // 2nd " after \n
                        state = LEXER_STATE_MULTILINE_STRING_END;
                        break;
                    } else if(nextChar == EOF){
                        return TOKEN_ERR_LEXICAL;
                    }
                    if (!BufferString_append_char(buffer_string, '\n'))
                        return TOKEN_ERR_INTERNAL;
                    if (!BufferString_append_char(buffer_string, '"'))
                        return TOKEN_ERR_INTERNAL;
                    ungetc(nextChar, source_file);
                    state = LEXER_STATE_MULTILINE_STRING;
                    break;
                
                case LEXER_STATE_MULTILINE_STRING_END:
                    if (nextChar == '"'){ // 3rd " after \n
                        return TOKEN_LITERAL_STRING;
                    } else if(nextChar == EOF){
                        return TOKEN_ERR_LEXICAL;
                    }
                    if (!BufferString_append_char(buffer_string, '\n'))
                        return TOKEN_ERR_INTERNAL;
                    if (!BufferString_append_char(buffer_string, '"'))
                        return TOKEN_ERR_INTERNAL;
                    if (!BufferString_append_char(buffer_string, '"'))
                        return TOKEN_ERR_INTERNAL;
                    ungetc(nextChar, source_file);
                    state = LEXER_STATE_MULTILINE_STRING;
                    break;

                case LEXER_STATE_MINUS_OR_ARROW:
                    if(nextChar == '>')
                        return TOKEN_ARROW;

                    ungetc(nextChar,source_file);
                    return TOKEN_OPERATOR_MINUS;

                case LEXER_STATE_LESS_POSSIBLE_EQUAL:
                    if(nextChar == '=')
                        return TOKEN_OPERATOR_LESS_THAN_OR_EQUAL;
                    
                    ungetc(nextChar,source_file);
                    return TOKEN_OPERATOR_LESS_THAN;

                case LEXER_STATE_GREATER_POSSIBLE_EQUAL:
                    if(nextChar == '=')
                        return TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL;
                        
                    ungetc(nextChar,source_file);
                    return TOKEN_OPERATOR_GREATER_THAN;
                        
                case LEXER_STATE_POSSIBLE_NOT_EQUALS:
                    if(nextChar == '=')
                        return TOKEN_OPERATOR_NOT_EQUALS;
                        
                    ungetc(nextChar,source_file);
                    return TOKEN_EXCLAMATION;

                case LEXER_STATE_POSSIBLE_EQUALS:
                    if(nextChar == '=')
                        return TOKEN_OPERATOR_EQUALS;
                        
                    ungetc(nextChar,source_file);
                    return TOKEN_ASSIGN;

                case LEXER_STATE_POSSIBLE_COALESCING:
                    if(nextChar == '?')
                        return TOKEN_NIL_COALESCING;
                        
                    ungetc(nextChar,source_file);
                    return TOKEN_QUESTION;
                
                default:
                    // sanity check for unimplemented state
                    assert(false);
                    break;
        }
    }
}
