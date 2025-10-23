#include <stdio.h>
#include <string.h>

#include "../../generic/brush.h"
#include "../../generic/map.h"


/* main parser function */
MapFile   parse_valve220_map(FILE *map_file);
/* TODO: metadata (.wad etc.) */

/* line parsers */
KeyValue  parse_valve220_map_keyvalue(char* line);
BrushSide parse_valve220_map_brushside(char* line);
