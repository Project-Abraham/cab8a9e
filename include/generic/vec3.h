#pragma once

typedef struct { float x, y, z; } vec3;

/* unary methods */
vec3 vec3_neg(vec3 vector);
/* binary methods */
vec3 vec3_add(vec3 lhs, vec3 rhs);
vec3 vec3_sub(vec3 lhs, vec3 rhs);
float vec3_dot(vec3 lhs, vec3 rhs);
