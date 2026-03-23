/* Using C89 Standard */
#pragma once

#include <stdio.h>


int open(char* path, FILE *file);
int read(FILE *file, int length, char *dest);
int read_until(FILE *file, int length, char terminator, char *dest);
/* error if EOF or length reached; pass if terminator hit */
/* TODO: consume whitespace / newline */
int close(FILE *file);

