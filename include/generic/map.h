# pragma once

#include "brush.h"


/* NOTE: key is typically limited to 32 chars long */
/* NOTE: value is typically limited to 1024 chars long */
typedef struct {
    char* key;
    char* value;
} KeyValue;


typedef struct {
    unsigned int  num_brushes;
    unsigned int  num_keyvalues;
    Brush        *brushes;
    KeyValue     *keyvalues;
} Entity;


typedef struct {
    unsigned int  num_entities;
    Entity       *entities;
} MapFile;
