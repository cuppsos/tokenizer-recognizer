#include "Common.h"

// Defines tokens list and token_count for list

Lexeme tokens[MAX_LEXEMES];
int token_count = 0;

const char* vartypes[] = { "int", "void" };
const char* binops[] = { "+", "*", "!=", "==", "%" };
const char* symbols[] = { "(", ")", "{", "}", "=", ",", ";" };


// Create a new Lexeme and increments the total count of tokens
void addToken(const char* lexeme, TokenType token) {
    if (token_count >= MAX_LEXEMES) return;
    tokens[token_count].lexeme = malloc(strlen(lexeme) + 1);
    strcpy(tokens[token_count].lexeme, lexeme);
    tokens[token_count].token = token;
    token_count++;
}

// For identifying if a token is NUMBER
int isNumber(const char* lexeme) {
    for (int i = 0; lexeme[i]; i++) {
        if (!isdigit(lexeme[i])) return 0;
    }
    return 1;
}

// For identifying if a token is VARTYPE
int isVartype(const char* lexeme) {
    return strcmp(lexeme, "int") ==0 || strcmp(lexeme, "void") == 0;
}

// For identifying if a token is BINOP
int isBinop(const char* lexeme) {
    for (int i = 0; i < 5; i++) {
        if (strcmp(lexeme, binops[i]) == 0) return 1;
    }
    return 0;
}

//For identifying if a token is WHILE_KEYWORD/RETURN_KEYWORD/or VARTYPE
int isReserved(const char* lexeme, TokenType* type) {
    if (strcmp(lexeme, "while") == 0) {
        *type = WHILE_KEYWORD; return 1;
    } else if (strcmp(lexeme, "return") == 0) {
        *type = RETURN_KEYWORD; return 1;
    } else if (isVartype(lexeme)) {
        *type = VARTYPE; return 1;
    }
    return 0;
}

// Correlates lexemes to tokens
TokenType classifyLexeme(const char* lexeme) {
    TokenType token;
    if (isReserved(lexeme, &token)) return token;
    if (isBinop(lexeme)) return BINOP;
    if (isNumber(lexeme)) return NUMBER;

    if (strcmp(lexeme, "(") == 0) return LEFT_PARENTHESIS;
    if (strcmp(lexeme, ")") == 0) return RIGHT_PARENTHESIS;
    if (strcmp(lexeme, "{") == 0) return LEFT_BRACKET;
    if (strcmp(lexeme, "}") == 0) return RIGHT_BRACKET;
    if (strcmp(lexeme, "=") == 0) return EQUAL;
    if (strcmp(lexeme, ",") == 0) return COMMA;
    if (strcmp(lexeme, ";") == 0) return EOL;

    return IDENTIFIER;
}

// For tokenizing the file
void tokenizeFile(const char* input_path) {
    // Opens file for reading
    FILE* f = fopen(input_path, "r");

    // Defines current char, next char, buffer for building lexeme, and index to the buffer
    char ch, next;
    char buffer[MAX_LEX_LEN];
    int i = 0;

    // Reads one char at a time until end of file
    while ((ch = fgetc(f)) != EOF) {
        // Skip whitespace
        if (isspace(ch)) {
            // If buidling lexeme end and classify
            if (i > 0) {
                buffer[i] = '\0';
                addToken(buffer, classifyLexeme(buffer));
                i = 0;
            }
            continue;
        }

        // Handle alphanumeric lexemes
        if (isalpha(ch) || isdigit(ch)) {
            buffer[i++] = ch;
            continue;
        }

        // If buidling lexeme end and classify
        if (i > 0) {
            buffer[i] = '\0';
            addToken(buffer, classifyLexeme(buffer));
            i = 0;
        }

        // Handle multi-char symbols
        if (ch == '=' || ch == '!') {
            next = fgetc(f);
            if (next == '=') {
                buffer[0] = ch;
                buffer[1] = '=';
                buffer[2] = '\0';
                addToken(buffer, BINOP);
            } else {
                ungetc(next, f);
                buffer[0] = ch;
                buffer[1] = '\0';
                addToken(buffer, classifyLexeme(buffer));
            }
        } else {
            // Single-character symbol
            buffer[0] = ch;
            buffer[1] = '\0';
            addToken(buffer, classifyLexeme(buffer));
        }
    }

    // Final if buidling lexeme end and classify
    if (i > 0) {
        buffer[i] = '\0';
        addToken(buffer, classifyLexeme(buffer));
    }
    fclose(f);
}

// Outputs the final list of tokens and their lexemes
void writeTokens(const char* output_path) {
    FILE* f = fopen(output_path, "w");

    for (int i = 0; i < token_count; i++) {
        const char* token_str;
        switch (tokens[i].token) {
            case LEFT_PARENTHESIS: token_str = "LEFT_PARENTHESIS"; break;
            case RIGHT_PARENTHESIS: token_str = "RIGHT_PARENTHESIS"; break;
            case LEFT_BRACKET: token_str = "LEFT_BRACKET"; break;
            case RIGHT_BRACKET: token_str = "RIGHT_BRACKET"; break;
            case WHILE_KEYWORD: token_str = "WHILE_KEYWORD"; break;
            case RETURN_KEYWORD: token_str = "RETURN_KEYWORD"; break;
            case EQUAL: token_str = "EQUAL"; break;
            case COMMA: token_str = "COMMA"; break;
            case EOL: token_str = "EOL"; break;
            case VARTYPE: token_str = "VARTYPE"; break;
            case IDENTIFIER: token_str = "IDENTIFIER"; break;
            case BINOP: token_str = "BINOP"; break;
            case NUMBER: token_str = "NUMBER"; break;
            default: token_str = "UNKNOWN"; break;
        }
        fprintf(f, "%s %s\n", token_str, tokens[i].lexeme);
    }
    fclose(f);
}

int main(int argc, char* argv[]) {
    if (argc == 3) {
        // Tokenize input file
        tokenizeFile(argv[1]);
        // Write tokens and their lexemes to output file
        writeTokens(argv[2]);

        // Free allocated memory
        for (int i = 0; i < token_count; i++) {
            free(tokens[i].lexeme);
        }
    }
    return 0;
}