#include <stdbool.h>
#include <stdlib.h>

#include "generic/map.h"


void free_MapFile(MapFile *map) {
    free(map->entities[0].brushes[0].sides);
    free(map->entities[0].brushes);
    /* TODO: free all material strings */
    free(map->entities[0].keyvalues);
    /* TODO: free all key & value strings */
    free(map->entities);
};
