#include <stdio.h>
#include <string.h>

#include "../generic/map.h"

/* NOTE: assumes 32-bit `int` */
typedef struct {
    int   hex_magic;
    char  txt_magic[8];
    char  version[4];
    int   num_lumps;
    char  birthday[8];
} BabiHeader;
/* 0xCAB8A9E0, 'CABBAGE', 'v1.0', 0..?, 'YYYYMMDD' */


/* NOTE: strings might not be null terminated */
typedef struct {
    char  lump_name[8];
    int   offset;
    int   length;
    char  variant[8];
} BabiLumpHeader;
/* NOTE: lump structures and parsers should be split up into other files */


/* Data Structures */
typedef struct {
    unsigned int    num_lumps;
    BabiLumpHeader  lump_headers[];
} Babi;


/* Functions */
Babi parse_babi(FILE *babi_file);
MapFile Babi_as_MapFile(Babi babi);
