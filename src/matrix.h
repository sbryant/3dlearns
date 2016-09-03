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
void mat4x4_mul(const mat4x4 *m, const mat4x4 *m2, mat4x4 *out);

mat4x4* mat4x4_make(void);
void mat4x4_init(mat4x4* r);
void mat4x4_cleanup(mat4x4* m);
void mat4x4_make_ident(mat4x4* m);
float* mat4x4_make_array(mat4x4* const m);

void mat4x4_print(mat4x4* m);

mat4x4* mat4x4_rotate(mat4x4* m, float angle, vec3* up);
void mat4x4_translate(mat4x4 *m, float x, float y, float z, mat4x4 *out);
void mat4x4_set_mat4x4(mat4x4 *m1, const mat4x4 *m2);
void mat4x4_scale(mat4x4* m, float x, float y, float z);
mat4x4* mat4x4_look_at(vec3 *pos, vec3 *center, vec3 *up);
mat4x4* mat4x4_perspective(float fovy, float aspect, float znear, float zfar);
mat4x4* mat4x4_camera_view(vec3 *pos, vec3 *dir, vec3 *up);
mat4x4* mat4x4_inverted_tr(mat4x4 *m);
