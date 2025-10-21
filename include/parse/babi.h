#include <stdio.h>
#include <string.h>


/* Data Structures */
typedef struct {
    unsigned int  num_brushes;
    unsigned int  num_entities;
    unsigned int  num_planes;
    unsigned int  num_strings;
} Babi;


/* Functions */
Babi *parse_babi(FILE *babi_file);
