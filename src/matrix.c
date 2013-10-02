#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"


#define mp_at(m,v,i) m->v[i]

vec4x1 *mat4x4_mul_vec4x1(mat4x4* const m, vec4x1* const v) {
    vec4x1 *v2 = (vec4x1*)calloc(1, sizeof(vec4x1));

    v2->x = (mp_at(m,x,0) * v->x) + (mp_at(m,y,0) * v->y) + (mp_at(m,z,0) * v->z) + (mp_at(m,w,0) * v->w);
    v2->y = (mp_at(m,x,1) * v->x) + (mp_at(m,y,1) * v->y) + (mp_at(m,z,1) * v->z) + (mp_at(m,w,1) * v->w);
    v2->z = (mp_at(m,x,2) * v->x) + (mp_at(m,y,2) * v->y) + (mp_at(m,z,2) * v->z) + (mp_at(m,w,2) * v->w);
    v2->w = (mp_at(m,x,3) * v->x) + (mp_at(m,y,3) * v->y) + (mp_at(m,z,3) * v->z) + (mp_at(m,w,3) * v->w);

    return v2;
}

mat4x4 *mat4x4_mul(mat4x4* const m, mat4x4* const m2) {
    mat4x4 *r = mat4x4_init(NULL);

    for(int i = 0; i < 4; i++) {
        r->x[i] = mp_at(m,x,i) * mp_at(m2, x, 0) + \
            mp_at(m, y, i) * mp_at(m2, x, 1) +         \
            mp_at(m, z, i) * mp_at(m2, x, 2) +         \
            mp_at(m, w, i) * mp_at(m2, x, 3);

        r->y[i] = mp_at(m,x,i) * mp_at(m2, y, 0) + \
            mp_at(m, y, i) * mp_at(m2, y, 1) +         \
            mp_at(m, z, i) * mp_at(m2, y, 2) +         \
            mp_at(m, w, i) * mp_at(m2, y, 3);

        r->z[i] = mp_at(m,x,i) * mp_at(m2, z, 0) + \
            mp_at(m, y, i) * mp_at(m2, z, 1) +         \
            mp_at(m, z, i) * mp_at(m2, z, 2) +         \
            mp_at(m, w, i) * mp_at(m2, z, 3);

        r->w[i] = mp_at(m,x,i) * mp_at(m2, w, 0) + \
            mp_at(m, y, i) * mp_at(m2, w, 1) +         \
            mp_at(m, z, i) * mp_at(m2, w, 2) +         \
            mp_at(m, w, i) * mp_at(m2, w, 3);
    }

    return r;
}

mat4x4* mat4x4_init(mat4x4* r) {
    if (!r)
        r = (mat4x4*)calloc(1, sizeof(mat4x4));

    r->x = (float*)calloc(4, sizeof(float));
    r->y = (float*)calloc(4, sizeof(float));
    r->z = (float*)calloc(4, sizeof(float));
    r->w = (float*)calloc(4, sizeof(float));

    return r;
}

mat4x4* mat4x4_make_ident(mat4x4* m) {
    mat4x4* r = !m ? mat4x4_init(NULL) : m;

    memset((void*)(r->x), 0, sizeof(float) * 4);
    memset((void*)(r->y), 0, sizeof(float) * 4);
    memset((void*)(r->z), 0, sizeof(float) * 4);
    memset((void*)(r->w), 0, sizeof(float) * 4);

    r->x[0] = 1.0;
    r->y[1] = 1.0;
    r->z[2] = 1.0;
    r->w[3] = 1.0;

    return r;
}

void mat4x4_print(mat4x4* m) {
    for(int i = 0; i < 4; i++) {
        printf("x[%d]:%.2f y[%d]:%.2f z[%d]%.2f w[%d]%.2f\n",
               i, m->x[i],
               i, m->y[i],
               i, m->z[i],
               i, m->w[i]);
    }
}
