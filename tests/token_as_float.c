#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse/token_value.h"


int main(int argc, char* argv[]) {
    char      line[2048];
    char*     token_string;
    TokenSet  tokens, float_tokens;
    int       i;
    float     f;
    
    token_string = malloc(2048);

    fgets(line, sizeof(line), stdin);
    tokenise(line, 2048, &tokens);

    for (i = 0; i < tokens.num_tokens; i++) {
        if (!tokenise_float(line, tokens.tokens[i], &float_tokens)) {
            printf("\n");
            continue;
        }
        if (!tokens_as_float(line, float_tokens, &f)) {
            printf("! failed to parse tokenised float\n");
        }

        int offset = tokens.tokens[i].offset;
        int length = tokens.tokens[i].length;
        strncpy(token_string, line + offset, length);
        strncpy(token_string + length, "\0", 1);
        printf("\"%s\" -> %f\n", token_string, f);
    }

    if (tokens.tokens != NULL) {
        free(tokens.tokens);
    }
    free(token_string);

    return 0;
}
