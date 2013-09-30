#pragma once
#include "vector.h"


/* Column Major */
typedef struct s_mat4x4 {
    float x[4];
    float y[4];
    float z[4];
    float w[4];
} mat4x4;

#define MAT4_IDENT {                            \
        { 1.0, 0.0, 0.0, 0.0 },                 \
            { 0.0, 1.0, 0.0, 0.0 },             \
                { 0.0, 0.0, 1.0, 0.0 },         \
                    { 0.0, 0.0, 0.0, 1.0 }      \
    };



vec4x1* mat4x4_mul_vec4x1(mat4x4* const m, vec4x1* const v);
mat4x4* mat4x4_mul(mat4x4* const m, mat4x4* const m2);

mat4x4* mat4x4_make_ident(mat4x4* const m);

void mat4x4_print(mat4x4* m);
