#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char* operators = "+-*/%=!<>|&";
static const char* specialCharacters = ",;{}()[]#.";

static FILE *file = NULL;

void initializeLexer(const char* filename) {
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening source file");
    }
}

int isKeyword(const char* str) {
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isOperator(const char* str) {
    const char* multi_ops[] = {
        "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", 
        "<=", ">=", "&&", "||", "!", "+=", "-=", "*=", "/=", 
        "++", "--", "&", "|"
    };
    int num_ops = sizeof(multi_ops) / sizeof(multi_ops[0]);
    for (int i = 0; i < num_ops; i++) {
        if (strcmp(str, multi_ops[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isSpecialCharacter(char ch) {
    return strchr(specialCharacters, ch) != NULL;
}

int isConstant(const char* str) {
    // Check for string or character literals
    if (str[0] == '"' || str[0] == '\'') {
        return 1;
    }
    // Check for numeric constants (integers or floats)
    int has_decimal = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            if (has_decimal) return 0; // Multiple decimals are invalid
            has_decimal = 1;
        } else if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int isIdentifier(const char* str) {
    if (!(isalpha(str[0]) || str[0] == '_')) {
        return 0;
    }
    for (int i = 1; str[i] != '\0'; i++) {
        if (!(isalnum(str[i]) || str[i] == '_')) {
            return 0;
        }
    }
    return !isKeyword(str);
}

void categorizeToken(Token* token) {
    if (isKeyword(token->lexeme)) {
        token->type = KEYWORD;
    } else if (isIdentifier(token->lexeme)) {
        token->type = IDENTIFIER;
    } else if (isConstant(token->lexeme)) {
        token->type = CONSTANT;
    } else if (isOperator(token->lexeme)) {
        token->type = OPERATOR;
    } else if (strlen(token->lexeme) == 1 && isSpecialCharacter(token->lexeme[0])) {
        token->type = SPECIAL_CHARACTER;
    } else {
        token->type = UNKNOWN;
    }
}

Token getNextToken() {
    Token token;
    memset(&token, 0, sizeof(Token));
    token.type = UNKNOWN;

    if (file == NULL) {
        return token;
    }

    int ch = fgetc(file);

    // Skip whitespace and comments
    while (ch != EOF) {
        if (isspace(ch)) {
            ch = fgetc(file);
            continue;
        }
        // Handle single-line (//) and multi-line (/* */) comments
        if (ch == '/') {
            int next_ch = fgetc(file);
            if (next_ch == '/') {
                while ((ch = fgetc(file)) != '\n' && ch != EOF);
                ch = fgetc(file);
                continue;
            } else if (next_ch == '*') {
                int prev = 0;
                while ((ch = fgetc(file)) != EOF) {
                    if (prev == '*' && ch == '/') break;
                    prev = ch;
                }
                ch = fgetc(file);
                continue;
            } else {
                ungetc(next_ch, file);
            }
        }
        break;
    }

    if (ch == EOF) {
        token.type = UNKNOWN;
        strcpy(token.lexeme, "");
        return token;
    }

    int i = 0;

    // 1. Handle String & Character Literals
    if (ch == '"' || ch == '\'') {
        char quote = ch;
        token.lexeme[i++] = ch;
        while ((ch = fgetc(file)) != EOF && ch != quote) {
            if (i < MAX_TOKEN_SIZE - 1) {
                token.lexeme[i++] = ch;
            }
        }
        if (ch == quote) {
            token.lexeme[i++] = quote;
        }
        token.lexeme[i] = '\0';
        token.type = CONSTANT;
        return token;
    }

    // 2. Handle Identifiers and Keywords
    if (isalpha(ch) || ch == '_') {
        token.lexeme[i++] = ch;
        while ((ch = fgetc(file)) != EOF && (isalnum(ch) || ch == '_')) {
            if (i < MAX_TOKEN_SIZE - 1) {
                token.lexeme[i++] = ch;
            }
        }
        ungetc(ch, file);
        token.lexeme[i] = '\0';
        categorizeToken(&token);
        return token;
    }

    // 3. Handle Numeric Constants
    if (isdigit(ch)) {
        token.lexeme[i++] = ch;
        while ((ch = fgetc(file)) != EOF && (isdigit(ch) || ch == '.')) {
            if (i < MAX_TOKEN_SIZE - 1) {
                token.lexeme[i++] = ch;
            }
        }
        ungetc(ch, file);
        token.lexeme[i] = '\0';
        token.type = CONSTANT;
        return token;
    }

    // 4. Handle Special Characters / Punctuators
    if (isSpecialCharacter(ch)) {
        token.lexeme[i++] = ch;
        token.lexeme[i] = '\0';
        token.type = SPECIAL_CHARACTER;
        return token;
    }

    // 5. Handle Operators (Supports multi-character operators like ==, ++, etc.)
    if (strchr(operators, ch) != NULL) {
        token.lexeme[i++] = ch;
        int next_ch = fgetc(file);
        char potential_op[3] = { (char)ch, (char)next_ch, '\0' };

        if (isOperator(potential_op)) {
            token.lexeme[i++] = next_ch;
        } else {
            ungetc(next_ch, file);
        }
        token.lexeme[i] = '\0';
        token.type = OPERATOR;
        return token;
    }

    // Fallback for unrecognized single characters
    token.lexeme[i++] = ch;
    token.lexeme[i] = '\0';
    token.type = UNKNOWN;
    return token;
}