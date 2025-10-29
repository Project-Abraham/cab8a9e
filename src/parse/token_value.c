#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parse/token_value.h"
#include "parse/tokens.h"


/* NOTE: both of these functions could be macros */
/* -- can't use `inline` w/ C89 anyway */
bool is_sign(char c) {
    return (c == '-' || c == '+');
}


bool is_decimal(char c) {
    return ((int)c >= (int)'0' && (int)c <= (int)'9');
}


float e(int power) {
    int i, outi;
    float outf;

    /* NOTE: power 0 counts as negative, skips outi assign & cast */
    if (power > 0) {
        outi = 1;
        for (i = 0; i < power; i++) {
            outi = outi * 10;
        }
        outf = (float)outi;
    } else {
        outf = 1.0;
        for (i = power; i < 0; i++) {
            outf = outf / 10;
        }
    }
    return outf;
}


/* approx regex: [+-]?[0-9]+(.[0-9]*)?([eE][+-]?[0-9]+)? */
/* -- split into 3 tokens */
/* -- sign base . base_decimal [eE] exp */
/* -- returns false if string token is not a float */
bool tokenise_float(char* string, Token token, TokenSet *out) {
    int   i, j, exponent_i;
    bool  decimal_hit, exponent_hit;

    if (token.length == 0) {
        return false;
    }

    out->num_tokens = 3;
    out->tokens = malloc(sizeof(Token) * 3);
    memset(out->tokens, 0, sizeof(Token) * 3);

    i = token.offset;
    j = 0;
    out->tokens[j].offset = i;

    decimal_hit = false;
    exponent_hit = false;
    exponent_i = 0;

    if (is_sign(string[i])) {
        i++;
    }
    while (i < token.offset + token.length) {
        if (!is_decimal(string[i])) {
            if ((string[i] == 'e' || string[i] == 'E') && !exponent_hit) {
                exponent_hit = true;
                exponent_i = i;
                out->tokens[j].length = i - out->tokens[j].offset;
                j = 2;
                out->tokens[j].offset = i + 1;
            } else if (string[i] == '.' && !decimal_hit && !exponent_hit) {
                decimal_hit = true;
                out->tokens[j].length = i - out->tokens[j].offset;
                j = 1;
                out->tokens[j].offset = i + 1;
            } else if (exponent_i == i - 1 && exponent_hit) {
                if (!is_sign(string[i])) {
                    return false;
                }
            } else {
                return false;
            }
        }
        i++;
    }
    out->tokens[j].length = i - out->tokens[j].offset;

    /* invalid edge cases */
    /* base only contains sign (valid, but we don't parse it) */
    if (out->tokens[0].length == 1 && is_sign(string[out->tokens[0].offset])) {
        return false;
    }
    /* exponent only */
    if (out->tokens[0].length == 0 && out->tokens[1].length == 0) {
        return false;
    }
    /* empty exponent token, despite exponent marker */
    if (exponent_hit && out->tokens[2].length == 0) {
        return false;
    }

    return true;
};


/* NOTE: incredibly inaccurate, but that's what you get for using floats */
bool tokens_as_float(char* string, TokenSet tokens, float* out) {
    int       base, decimali, exp;
    float     decimalf;

    base = 0;
    if (tokens.tokens[0].length != 0) {
        if (!token_as_int(string, tokens.tokens[0], &base)) {
            return false;  /* invalid base */
        }
    } else {
        base = 0;
    }

    decimali = 0;
    decimalf = 0;
    if (tokens.tokens[1].length != 0) {
        if (!token_as_int(string, tokens.tokens[1], &decimali)) {
            return false;  /* invalid decimal */
        }
        decimalf = (float)decimali / e(tokens.tokens[1].length);
    }

    exp = 0;
    if (tokens.tokens[2].length != 0) {
        if (!token_as_int(string, tokens.tokens[2], &exp)) {
            return false;  /* invalid exponent */
        }
    }

    *out = ((float)base + decimalf) * e(exp);

    free(tokens.tokens);
    tokens.tokens = NULL;
    tokens.num_tokens = 0;

    return true;
};


bool token_as_int(char* string, Token token, int *out) {
    int   i, sign;

    if (token.length == 0) {
        return false;
    }

    i = token.offset;
    sign = +1;
    switch (string[i]) {
        case '-': sign = -1;
        case '+': i++;
            if (token.length == 1) {
                return false;  /* sign w/ no digits */
            }
        default: break;
    }

    *out = 0;
    while (i < token.offset + token.length) {
        if (!is_decimal(string[i])) {
            return false;  /* invalid char */
        }
        *out = (*out * 10) + ((int)string[i] - (int)'0');
        i++;
    }

    *out = *out * sign;
    return true;
}
