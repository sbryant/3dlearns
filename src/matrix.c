#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"

#define mp_at(m,v,i) m->v[i]

vec4 *mat4x4_mul_vec4(mat4x4* const m, vec4* const v) {
    vec4 *v2 = (vec4*)calloc(1, sizeof(vec4));

    v2->x = (mp_at(m,x,0) * v->x) + (mp_at(m,y,0) * v->y) + (mp_at(m,z,0) * v->z) + (mp_at(m,w,0) * v->w);
    v2->y = (mp_at(m,x,1) * v->x) + (mp_at(m,y,1) * v->y) + (mp_at(m,z,1) * v->z) + (mp_at(m,w,1) * v->w);
    v2->z = (mp_at(m,x,2) * v->x) + (mp_at(m,y,2) * v->y) + (mp_at(m,z,2) * v->z) + (mp_at(m,w,2) * v->w);
    v2->w = (mp_at(m,x,3) * v->x) + (mp_at(m,y,3) * v->y) + (mp_at(m,z,3) * v->z) + (mp_at(m,w,3) * v->w);

    return v2;
}


mat4x4 *mat4x4_translate(mat4x4* m, float x, float y, float z) {
    mat4x4* r = mat4x4_make_ident(NULL);
    r->w[0] = x;
    r->w[1] = y;
    r->w[2] = z;

    mat4x4* res = mat4x4_mul(m, r);

    mat4x4_cleanup(r);

    return res;
}

void mat4x4_scale(mat4x4* m, float x, float y, float z) {
    m->x[0] = x * m->x[0];
    m->y[1] = y * m->y[1];
    m->z[2] = z * m->z[2];
}

mat4x4 *mat4x4_mul(mat4x4* const m, mat4x4* const m2) {
    mat4x4 *r = mat4x4_make_ident(NULL);

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

    r->x = (r->m);
    r->y = (r->m)+4;
    r->z = (r->m)+8;
    r->w = (r->m)+12;

    return r;
}

mat4x4* mat4x4_make_ident(mat4x4* m) {
    mat4x4* r = !m ? mat4x4_init(NULL) : m;

    memset((void*)&(r->m), 0, sizeof(float) * 16);

    r->x[0] = 1.0;
    r->y[1] = 1.0;
    r->z[2] = 1.0;
    r->w[3] = 1.0;

    return r;
}

float* mat4x4_make_array(mat4x4* const m) {
    float *r = (float*)calloc(16, sizeof(float));

    memcpy((void*)r, &m->m, sizeof(float) * 16);
    return r;
}

void mat4x4_print(mat4x4* m) {
    for(int i = 0; i < 4; i++) {
        printf("x[%d]:%.6f y[%d]:%.6f z[%d]%.6f w[%d]%.6f\n",
               i, m->x[i],
               i, m->y[i],
               i, m->z[i],
               i, m->w[i]);
    }
}

void mat4x4_cleanup_comp(mat4x4* m) {
    m->x = NULL;
    m->y = NULL;
    m->z = NULL;
    m->w = NULL;
}

mat4x4* mat4x4_rotate(mat4x4* m, float angle, vec3* axis) {
    mat4x4 r; mat4x4_init(&r); mat4x4_make_ident(&r);

    float c = cosf(angle);
    float s = sinf(angle);

    if (axis->x) {
        r.y[1] = c;
        r.y[2] = s;

        r.z[1] = -s;
        r.z[2] = c;
    }

    if (axis->y) {
        r.x[0] = c;
        r.x[2] = -s;

        r.z[0] = s;
        r.z[2] = c;
    }


    if (axis->z) {
        r.x[0] = c;
        r.x[1] = s;

        r.y[0] = -s;
        r.y[1] = c;
    }

    mat4x4 *res = mat4x4_mul(&r, m);
    mat4x4_cleanup_comp(&r);

    return res;
}

void mat4x4_cleanup(mat4x4* m) {
    mat4x4_cleanup_comp(m);
    free(m);
}

void mat4x4_set_mat4x4(mat4x4 *m1, const mat4x4 *m2) {
    memcpy(&(m1->m), &(m2->m), sizeof(float) * 16);
}
