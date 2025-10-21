#include "texture.h"
#include "vec2.h"
#include "vec3.h"


float ProjAxis_project(ProjAxis proj, vec3 point) {
    return (vec3_dot(point, proj.axis) + proj.offset) * proj.scale;
};


TexProj TexProj_from_normal(vec3 normal) {
    int axis = 2;  /* default to Z */
    if (normal.x != normal.y && normal.x != -normal.y) {
        float best = 0;
        float normal_[3] = { normal.x, normal.y, normal.z };
        for (int i = 0; i < 3; i++) {
            float score = normal_[i];
            if (score < 0) {  /* absolute value (+ve only) */
                score = -score;
            }
            if (score >= best) {
                best = score;
                axis = i;
            }
        }
    }
    vec3 local_x;
    if (axis == 0) { local_x = (vec3) { 0, 1, 0 }; }
    else           { local_x = (vec3) { 1, 0, 0 }; }
    vec3 local_y;
    if (axis != 2) { local_y = (vec3) { 0, 0, -1 }; }
    else           { local_y = (vec3) { 0, -1, 0 }; }
    ProjAxis s = { local_x, 0, 1 };
    ProjAxis t = { local_y, 0, 1 };
    TexProj out = { s, t, 0 };
    return out;
};


vec2 TexProj_uv_at(TexProj st, vec3 point) {
    /* NOTE: not applying rotation */
    float u = ProjAxis_project(st.s, point);
    float v = ProjAxis_project(st.t, point);
    vec2 out = { u, v };
    return out;
};
