#pragma once

#include "vec2.h"
#include "vec3.h"


/* ProjectionAxis */
typedef struct {
    vec3   axis;
    float  offset;
    float  scale;
} ProjAxis;


float ProjAxis_project(ProjAxis proj, vec3 point);


/* TextureProjection */
typedef struct {
    ProjAxis  s;
    ProjAxis  t;
    float     rotation;
} TexProj;


TexProj TexProj_from_normal(vec3 normal);
vec2 TexProj_uv_at(TexProj st, vec3 point);
