#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "generic/brush.h"
#include "generic/map.h"
#include "parse/tokens.h"


/* main parser function */
bool parse_valve220_map(FILE *map_file, MapFile *out);

/* line parsers */
bool parse_valve220_map_keyvalue(char* line, TokenSet tokens, KeyValue *out);
bool parse_valve220_map_brushside(char* line, TokenSet tokens, BrushSide *out);
