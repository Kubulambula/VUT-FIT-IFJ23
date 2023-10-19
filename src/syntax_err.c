#include <stdio.h>
#include "error.h"

void err_exit(const char* err_message, int line){
    fprintf(stderr, "error | line %d : ", line);
    fprintf(stderr, err_message);
    exit(ERR_SYNTAX);
}