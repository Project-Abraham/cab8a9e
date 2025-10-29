#include <stdlib.h>

#include "parse/tokens.h"


void tokenise(char* line, int length, TokenSet* out) {
    Token  token;
    int    i;

    out->num_tokens = 0;
    out->tokens = malloc(sizeof(Token) * 4);
    token.offset = 0;
    token.length = 0;
    i = 0;
    while (i < length) {
        switch (line[i]) {
            case ' ':
            case '\t':
            case '\n':
                if (token.length != 0) {
                    out->tokens[out->num_tokens] = token;
                    out->num_tokens++;
                    if (out->num_tokens % 4 == 0) {
                        out->tokens = realloc(out->tokens,
                            sizeof(Token) * (out->num_tokens + 4));
                    }
                }
                token.offset = i + 1;
                token.length = 0;
                break;
            case '\0':
                if (token.length != 0) {
                    out->tokens[out->num_tokens] = token;
                    out->num_tokens++;
                }
                if (out->num_tokens > 0) {
                    out->tokens = realloc(out->tokens,
                        sizeof(Token) * out->num_tokens);
                } else {
                    free(out->tokens);
                    out->tokens = NULL;  /* mark as freed */
                }
                return;
            default:
                token.length++;
        }
        i++;
    }
}
