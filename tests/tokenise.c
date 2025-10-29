#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse/tokens.h"


int main(int argc, char* argv[]) {
    char      line[2048];
    TokenSet  tokens;
    int       i;

    fgets(line, sizeof(line), stdin);
    tokenise(line, 2048, &tokens);

    printf("tokens.num_tokens = %d\n", tokens.num_tokens);
    for (i = 0; i < tokens.num_tokens; i++) {
        printf("  tokens.tokens[%03d] = { .offset=%d, .length=%d };\n",
            i, tokens.tokens[i].offset, tokens.tokens[i].length);
        /* print token string */
        /* NOTE: we could just replace whitespace w/ NULL */
        /* printf((token_string + offset)) */
        int offset = tokens.tokens[i].offset;
        int length = tokens.tokens[i].length;
        char* token_string = malloc(length + 1);
        strncpy(token_string, line + offset, length);
        strncpy(token_string + length, "\0", 1);
        printf("  \"%s\"\n", token_string);
        free(token_string);
    }

    if (tokens.tokens != NULL) {
        free(tokens.tokens);
    }

    return 0;
}
