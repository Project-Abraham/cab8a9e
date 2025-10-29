#pragma once

#include <stdbool.h>

#include "parse/tokens.h"


bool tokenise_float(char* string, Token token, TokenSet *out);
bool tokens_as_float(char* string, TokenSet tokens, float *out);
bool token_as_int(char* string, Token token, int *out);
