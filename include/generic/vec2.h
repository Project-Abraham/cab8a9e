#pragma once

typedef struct { float x, y; } vec2;

/* unary methods */
vec2 vec2_neg(vec2 vector);
/* binary methods */
vec2 vec2_add(vec2 lhs, vec2 rhs);
vec2 vec2_sub(vec2 lhs, vec2 rhs);
float vec2_dot(vec2 lhs, vec2 rhs);
