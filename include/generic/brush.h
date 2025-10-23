#pragma once

#include "texture.h"
#include "vec3.h"


typedef struct {
    vec3   normal;
    float  distance;
} Plane;


Plane Plane_from_points(vec3 A, vec3 B, vec3 C);


typedef struct {
    char*    material;
    Plane    plane;
    TexProj  projection;
} BrushSide;


typedef struct {
    unsigned int  num_sides;
    BrushSide    *sides;
} Brush;
