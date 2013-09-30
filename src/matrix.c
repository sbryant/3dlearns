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

mat4x4* mat4x4_make_ident(mat4x4* m) {
    mat4x4* r = m;
    int zero = 1;

    if (!r) {
        r = (mat4x4*)calloc(1, sizeof(mat4x4));
        zero = 0;
    }

    if (zero)
        memset((void*)r, 0, sizeof(mat4x4));

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
