#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parse/map/valve220.h"
#include "parse/tokens.h"
#include "parse/token_value.h"


bool parse_valve220_map(FILE *map_file, MapFile *out) {
    char       line[2048];
    TokenSet   tokens;
    char       first_char;
    int        node_depth;
    Entity     entity;
    Brush      brush;
    KeyValue   keyvalue;
    BrushSide  brushside;

    out->entities = malloc(sizeof(Entity) * 16);
    entity.brushes = malloc(sizeof(Brush) * 4);
    entity.keyvalues = malloc(sizeof(KeyValue) * 4);
    brush.sides = malloc(sizeof(BrushSide) * 6);

    node_depth = 0;
    do {
        /* tokenise line */
        fgets(line, 2048, map_file);
        tokenise(line, 2048, &tokens);
        first_char = '\0';
        if (tokens.num_tokens > 0) {
            first_char = line[tokens.tokens[0].offset];
        }
        /* parse line */
        if (first_char == '\0' || first_char == '/' || first_char == '\\') {
            continue;  /* blank line or comment, skip */
        } else if (first_char == '{') {  /* a single token (1 char) */
            node_depth++;
            if (node_depth >= 2) {
                /* TODO: free everything */
                return false;  /* too deep */
            }
        } else if (first_char == '}') {  /* a single token (1 char) */
            node_depth--;
            if (node_depth == 1) {
                out->entities[out->num_entities] = entity;
                out->num_entities++;
                /* TODO: realloc entity.keyvalues (fit) */
                /* TODO: realloc out->entities (overflow) */
                /* new entity */
                entity.num_brushes = 0;
                entity.num_keyvalues = 0;
                entity.brushes = malloc(sizeof(Brush) * 4);
                entity.keyvalues = malloc(sizeof(KeyValue) * 4);
            } else if (node_depth == 2) {
                entity.brushes[entity.num_brushes] = brush;
                entity.num_brushes++;
                /* TODO: realloc brush.sides (fit) */
                /* TODO: realloc brushes (overflow) */
                /* new brush */
                brush.num_sides = 0;
                brush.sides = malloc(sizeof(BrushSide) * 6);
            }
        } else if (parse_valve220_map_keyvalue(line, tokens, &keyvalue)) {
            entity.keyvalues[entity.num_keyvalues] = keyvalue;
            entity.num_keyvalues++;
            /* TODO: realloc entity.keyvalues (overflow) */
        } else if (parse_valve220_map_brushside(line, tokens, &brushside)) {
            brush.sides[brush.num_sides] = brushside;
            brush.num_sides++;
            /* TODO: realloc brush.sides (overflow) */
        } else {
            /* TODO: free everything */
            return false;  /* unexpected line */
        }
        /* cleanup */
        if (tokens.tokens != NULL) {
            free(tokens.tokens);
        }
    } while (line[0] != '\0');

    if (node_depth != 0) {
        return false;  /* unclosed node(s) */
    }

    return true;
};


/* "key" "value" */
bool parse_valve220_map_keyvalue(char* line, TokenSet tokens, KeyValue *out) {
    int offset, length;

    if (tokens.num_tokens != 2) {
        return false;
    }

    #define RETURN_FALSE_IF_NO_QUOTE \
        if (!(line[offset] == '\'' || line[offset] == '\"')) return false

    offset = tokens.tokens[0].offset;
    RETURN_FALSE_IF_NO_QUOTE;

    offset += tokens.tokens[0].length;
    RETURN_FALSE_IF_NO_QUOTE;

    offset = tokens.tokens[1].offset;
    RETURN_FALSE_IF_NO_QUOTE;

    offset += tokens.tokens[1].length;
    RETURN_FALSE_IF_NO_QUOTE;

    #undef RETURN_FALSE_IF_NO_QUOTE

    offset = tokens.tokens[0].offset + 1;
    length = tokens.tokens[0].length - 2;
    out->key = malloc(length + 1);
    strncpy(out->key, line + offset, length);
    strncpy(out->key + length, "\0", 1);

    offset = tokens.tokens[1].offset + 1;
    length = tokens.tokens[1].length - 2;
    out->value = malloc(length + 1);
    strncpy(out->value, line + offset, length);
    strncpy(out->value + length, "\0", 1);

    return true;
};


/*
( Ax Ay Az ) ( Bx By Bz ) ( Cx Cy Cz ) material
[ Sx Sy Sz Soffset ] [ Tx Ty Tz Toffset ] rotation Sscale Tscale
*/
bool parse_valve220_map_brushside(char* line, TokenSet tokens, BrushSide *out) {
    int       offset, length;

    if (tokens.num_tokens != 31) {
        return false;
    }

    #define ASSERT_TOKEN_IS_CHAR(i, c) \
        if (!( \
            tokens.tokens[i].length == 1 \
         && line[tokens.tokens[i].offset] == c)) \
            return false

    ASSERT_TOKEN_IS_CHAR(0, '(');
    ASSERT_TOKEN_IS_CHAR(4, ')');
    ASSERT_TOKEN_IS_CHAR(5, '(');
    ASSERT_TOKEN_IS_CHAR(9, ')');
    ASSERT_TOKEN_IS_CHAR(10, '(');
    ASSERT_TOKEN_IS_CHAR(14, ')');
    ASSERT_TOKEN_IS_CHAR(16, '[');
    ASSERT_TOKEN_IS_CHAR(21, ']');
    ASSERT_TOKEN_IS_CHAR(22, '[');
    ASSERT_TOKEN_IS_CHAR(27, ']');

    #undef ASSERT_TOKEN_IS_CHAR

    vec3 A, B, C;
    float rotation;

    #define TOKEN_TO_FLOAT(i, f) \
        if (!token_as_float(line, tokens.tokens[i], &f)) \
            return false

    TOKEN_TO_FLOAT( 1, A.x);
    TOKEN_TO_FLOAT( 2, A.y);
    TOKEN_TO_FLOAT( 3, A.z);
    TOKEN_TO_FLOAT( 6, B.x);
    TOKEN_TO_FLOAT( 7, B.y);
    TOKEN_TO_FLOAT( 8, B.z);
    TOKEN_TO_FLOAT(11, C.x);
    TOKEN_TO_FLOAT(12, C.y);
    TOKEN_TO_FLOAT(13, C.z);
    out->plane = Plane_from_points(A, B, C);

    offset = tokens.tokens[15].offset;
    length = tokens.tokens[15].length;
    out->material = malloc(length + 1);
    strncpy(out->material, line + offset, length);
    strncpy(out->material + length, "\0", 1);

    TOKEN_TO_FLOAT(17, out->projection.s.axis.x);
    TOKEN_TO_FLOAT(18, out->projection.s.axis.y);
    TOKEN_TO_FLOAT(19, out->projection.s.axis.z);
    TOKEN_TO_FLOAT(20, out->projection.s.offset);
    TOKEN_TO_FLOAT(23, out->projection.t.axis.x);
    TOKEN_TO_FLOAT(24, out->projection.t.axis.y);
    TOKEN_TO_FLOAT(25, out->projection.t.axis.z);
    TOKEN_TO_FLOAT(26, out->projection.t.offset);
    TOKEN_TO_FLOAT(28, rotation);  /* currently unused */
    TOKEN_TO_FLOAT(29, out->projection.s.scale);
    TOKEN_TO_FLOAT(30, out->projection.t.scale);

    #undef TOKEN_TO_FLOAT

    return true;
};
