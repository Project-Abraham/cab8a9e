#pragma once

#include <stdbool.h>

#include "parse/tokens.h"


bool token_as_float(char* string, Token token, float *out);
bool token_as_int(char* string, Token token, int *out);
