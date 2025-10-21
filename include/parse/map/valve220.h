#include <stdio.h>
#include <string.h>


/* Data Structures */
typedef struct {
    unsigned int  num_entities;
    unsigned int  num_brushes;
    unsigned int  num_brush_entities;
    /* TODO: pointers to brushes, entities & BrushEntities */
} Valve220Map;


/* Functions */
Valve220Map *parse_valve220_map(FILE *map_file);
