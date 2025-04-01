#include <stdio.h>
#include <string.h>


/* Data Structures */
struct Babi {
    unsigned int  num_brushes;
    unsigned int  num_entities;
    unsigned int  num_planes;
    unsigned int  num_strings;
};


/* Functions */
Babi *parse_babi(FILE *babi_file);
