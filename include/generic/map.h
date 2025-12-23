# pragma once

#include "brush.h"


/* NOTE: key is typically limited to 32 chars long */
/* NOTE: value is typically limited to 1024 chars long */
typedef struct {
    unsigned int  key;  /* index into MapFile.strings */
    unsigned int  value;
} KeyValue;


typedef struct {
    unsigned int  num_keyvalues;
    unsigned int  first_keyvalue;
    unsigned int  num_brushes;
    unsigned int  first_brush;
} Entity;


typedef struct {
    unsigned int  num_entities;
    unsigned int  num_brushes;
    /* brushsides? */
    unsigned int  num_keyvalues;
    unsigned int  strings_len;
    Entity       *entities;
    Brush        *brushes;
    KeyValue     *keyvalues;
    char         *strings;
} MapFile;


void free_MapFile(MapFile *map);
