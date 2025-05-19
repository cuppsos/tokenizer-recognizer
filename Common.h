#ifndef COMMON_H
#define COMMON_H

//includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Defines
#define MAX_LEXEMES 1024
#define MAX_LEX_LEN 256

// Token classes
typedef enum {
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    LEFT_BRACKET, RIGHT_BRACKET,
    WHILE_KEYWORD, RETURN_KEYWORD,
    EQUAL, COMMA, EOL, VARTYPE,
    IDENTIFIER, BINOP, NUMBER
} TokenType;

// Correlates each lexeme with a token class
typedef struct {
    char* lexeme;
    TokenType token;
} Lexeme;

#endif