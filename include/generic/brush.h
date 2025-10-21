#pragma once

#include "texture.h"
#include "vec3.h"


typedef struct {
    vec3   normal;
    float  distance;
} Plane;


Plane Plane_from_points(vec3 A, vec3 B, vec3 C);


struct BrushSide {
    char*    material;
    Plane    plane;
    TexProj  projection;
};


struct Brush {
    int  num_sides;
    struct BrushSide *sides;
};
