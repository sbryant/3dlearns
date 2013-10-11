#pragma once
#include "vector.h"

#ifndef PI
#define PI 3.141592653589793
#endif

/* Column Major */
typedef struct s_mat4x4 {
    float m[16];
    float *x;
    float *y;
    float *z;
    float *w;
} mat4x4;

vec4* mat4x4_mul_vec4(mat4x4* const m, vec4* const v);
mat4x4* mat4x4_mul(mat4x4* const m, mat4x4* const m2);

mat4x4* mat4x4_init(mat4x4* r);
void mat4x4_cleanup(mat4x4* m);
mat4x4* mat4x4_make_ident(mat4x4* m);
float* mat4x4_make_array(mat4x4* const m);

void mat4x4_print(mat4x4* m);

mat4x4* mat4x4_rotate(mat4x4* m, float angle, vec3* up);
mat4x4 *mat4x4_translate(mat4x4* m, float x, float y, float z);
