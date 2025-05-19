#include "Common.h"
// Forward declarations for functions
void parse();
void parseFunction();
void parseHeader();
void parseArgDecl();
void parseBody();
void parseBody();
void parseStatement();
void parseStatementList();
void parseWhileLoop();
void parseReturn();
void parseAssignment();
void parseExpression();
void parseTerm();

// Defines tokens list, token_count, and current token
Lexeme tokens[MAX_LEXEMES];
int token_count = 0;
int current = 0;

// Pointer to outfile
FILE* out;

// Opens the tokenized output file from Tokenizer and stores them for parsing
void loadTokens(const char* input_path) {
    FILE* f = fopen(input_path, "r");
    if (!f) exit(0);

    char token_str[64];
    char lexeme[256];

    while (fscanf(f, "%s %s", token_str, lexeme) == 2) {
        TokenType type;

        if (strcmp(token_str, "LEFT_PARENTHESIS") == 0) type = LEFT_PARENTHESIS;
        else if (strcmp(token_str, "RIGHT_PARENTHESIS") == 0) type = RIGHT_PARENTHESIS;
        else if (strcmp(token_str, "LEFT_BRACKET") == 0) type = LEFT_BRACKET;
        else if (strcmp(token_str, "RIGHT_BRACKET") == 0) type = RIGHT_BRACKET;
        else if (strcmp(token_str, "WHILE_KEYWORD") == 0) type = WHILE_KEYWORD;
        else if (strcmp(token_str, "RETURN_KEYWORD") == 0) type = RETURN_KEYWORD;
        else if (strcmp(token_str, "EQUAL") == 0) type = EQUAL;
        else if (strcmp(token_str, "COMMA") == 0) type = COMMA;
        else if (strcmp(token_str, "EOL") == 0) type = EOL;
        else if (strcmp(token_str, "VARTYPE") == 0) type = VARTYPE;
        else if (strcmp(token_str, "IDENTIFIER") == 0) type = IDENTIFIER;
        else if (strcmp(token_str, "BINOP") == 0) type = BINOP;
        else if (strcmp(token_str, "NUMBER") == 0) type = NUMBER;

        tokens[token_count].token = type;
        tokens[token_count].lexeme = malloc(strlen(lexeme) + 1);
        strcpy(tokens[token_count].lexeme, lexeme);
        token_count++;
    }

    fclose(f);
}

// Syntax error detecting
void syntaxError(const char* rule, const char* expected, const char* actual) {
    fprintf(out, "Error: In grammar rule %s, expected token #%d to be %s but was %s\n",
        rule, (current+1), expected, actual);
    exit(0);
}

// Non-terminal error detecting
void nonterminalError(const char* rule, const char* expected_nonterminal) {
    fprintf(out, "Error: In grammar rule %s, expected a valid %s non-terminal to be present but was not.\n",
        rule, expected_nonterminal);
    exit(0);
}

// Verifies that the current token is the expected one
int match(const char* rule, TokenType expected) {
    if (current >= token_count) {
        syntaxError("EOF", "token", "none");
    }

    if (tokens[current].token == expected) {
        current++;
        return 1;
    }

    // Convert token enums to string for error reporting
    const char* token_names[] = {
        "LEFT_PARENTHESIS", "RIGHT_PARENTHESIS", "LEFT_BRACKET", "RIGHT_BRACKET",
        "WHILE_KEYWORD", "RETURN_KEYWORD", "EQUAL", "COMMA", "EOL",
        "VARTYPE", "IDENTIFIER", "BINOP", "NUMBER"
    };

    syntaxError(rule, token_names[expected], token_names[tokens[current].token]);
    return 0; // not reached
}

// Parse functions for EBNF
void parseFunction() {

    if (current >= token_count) {
        nonterminalError("function", "function");
    }

    parseHeader();
    parseBody();
}
void parseHeader() {
    if (current >= token_count) {
        nonterminalError("header", "header");
    }

    match("header", VARTYPE);
    match("header", IDENTIFIER);
    match("header", LEFT_PARENTHESIS);

    // Optional arg-decl
    if (tokens[current].token == VARTYPE) {
        parseArgDecl();
    }

    match("header", RIGHT_PARENTHESIS);
}
void parseArgDecl() {
    match("arg-decl", VARTYPE);
    match("arg-decl", IDENTIFIER);

    while (tokens[current].token == COMMA) {
        match("arg-decl", COMMA);
        match("arg-decl", VARTYPE);
        match("arg-decl", IDENTIFIER);
    }
}
void parseBody() {
    match("body", LEFT_BRACKET);

    // Optional statement-list
    if (tokens[current].token == WHILE_KEYWORD ||
        tokens[current].token == RETURN_KEYWORD ||
        tokens[current].token == IDENTIFIER) {
        parseStatementList();
    }

    match("body", RIGHT_BRACKET);
}
void parseStatementList() {
    parseStatement();

    while (tokens[current].token == WHILE_KEYWORD ||
           tokens[current].token == RETURN_KEYWORD ||
           tokens[current].token == IDENTIFIER) {
        parseStatement();
    }
}
void parseStatement() {
    TokenType t = tokens[current].token;

    if (t == WHILE_KEYWORD) {
        parseWhileLoop();
    } else if (t == RETURN_KEYWORD) {
        parseReturn();
    } else if (t == IDENTIFIER) {
        parseAssignment();
    } else {
        nonterminalError("statement", "statement");
    }
}
void parseWhileLoop() {
    match("while-loop", WHILE_KEYWORD);
    match("while-loop", LEFT_PARENTHESIS);
    parseExpression();
    match("while-loop", RIGHT_PARENTHESIS);
    parseBody();
}
void parseReturn() {
    match("return", RETURN_KEYWORD);
    parseExpression();
    match("return", EOL);
}
void parseAssignment() {
    match("assignment", IDENTIFIER);
    match("assignment", EQUAL);
    parseExpression();
    match("assignment", EOL);
}
void parseExpression() {
    if (tokens[current].token == LEFT_PARENTHESIS) {
        match("expression", LEFT_PARENTHESIS);
        parseExpression();
        match("expression", RIGHT_PARENTHESIS);
    } else {
        parseTerm();
        while (tokens[current].token == BINOP) {
            match("expression", BINOP);
            parseTerm();
        }
    }
}
void parseTerm() {
    TokenType t = tokens[current].token;

    if (t == IDENTIFIER) {
        match("term", IDENTIFIER);
    } else if (t == NUMBER) {
        match("term", NUMBER);
    } else {
        nonterminalError("term", "term");
    }
}
void parse() {
    parseFunction();  // top-level rule
}

int main(int argc, char* argv[]) {
    if (argc == 3) {
        // Read token lexeme pairs from tokenizer output
        loadTokens(argv[1]);  
        out = fopen(argv[2], "w");

        // Read token-lexeme pairs from tokenizer output
        parse();

        if (current != token_count) {
            // Not all tokens were consumed — this is a syntax error
            fprintf(out, "Error: Only consumed %d of the %d given tokens\n", current, token_count);
        } else {
            // Successfully parsed all tokens
            fprintf(out, "PARSED!!!\n");
        }

        fclose(out);

        // Free allocated memory
        for (int i = 0; i < token_count; i++) {
            free(tokens[i].lexeme);
        }
    }
    return 0;
}
