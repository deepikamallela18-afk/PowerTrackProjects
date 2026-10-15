#include <stdio.h>
#include <string.h>
#include "lexer.h"

const char* getTokenTypeName(TokenType type) {
    switch (type) {
        case KEYWORD:           return "Keyword";
        case IDENTIFIER:        return "Identifier";
        case OPERATOR:          
        case SPECIAL_CHARACTER: return "Operator";
        case CONSTANT:          return "Literal";
        case UNKNOWN:           return "Unknown";
        default:                return "Unknown";
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <c file>\n", argv[0]);
        return 1;
    }

    printf("Open : %s : Success\n", argv[1]);
    printf("Parsing : %s : Started\n\n", argv[1]);

    initializeLexer(argv[1]);

    Token token;
    while (1) {
        token = getNextToken();

        if (token.type == UNKNOWN && token.lexeme[0] == '\0') {
            break;
        }

        if (token.type == UNKNOWN) {
            fprintf(stderr, "Lexical Error: Unrecognized character or token '%s'\n", token.lexeme);
            break;
        }

        printf("%-11s : %s\n", getTokenTypeName(token.type), token.lexeme);
    }

    printf("\nParsing : %s : Done\n", argv[1]);

    return 0;
}