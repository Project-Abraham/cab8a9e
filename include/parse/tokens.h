#pragma once


typedef struct {
    int  offset;
    int  length;
} Token;


typedef struct {
    int    num_tokens;
    Token *tokens;
} TokenSet;


void tokenise(char* line, int length, TokenSet* out);
