#include "vector.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

float vec4_length(vec4* const v) {
    return sqrtf((v->x * v->x) +
                 (v->y * v->y) +
                 (v->z * v->z) +
                 (v->w * v->w));
}

float vec4_length_square(vec4* const v) {
    return (v->x * v->x) +
        (v->y * v->y) +
        (v->z * v->z) +
        (v->w * v->w);
}

vec4* vec4_add(vec4* const v1, vec4* const v2) {
    vec4 *vr = (vec4*)calloc(1, sizeof(vec4));

    vr->x = v1->x + v2->x;
    vr->y = v1->y + v2->y;
    vr->z = v1->z + v2->z;
    vr->w = v1->w + v2->w;

    return vr;
}

vec4* vec4_sub(vec4* const v1, vec4* const v2) {
    vec4 *vr = (vec4*)calloc(1, sizeof(vec4));

    vr->x = v1->x - v2->x;
    vr->y = v1->y - v2->y;
    vr->z = v1->z - v2->z;
    vr->w = v1->w - v2->w;

    return vr;
}

/* Vec4 scaling functions */

vec4 *vec4_mul_scalar(vec4* const v, float const s) {
    vec4* v2 = (vec4 *)calloc(1, sizeof(vec4));

    v2->x = v->x * s;
    v2->y = v->y * s;
    v2->z = v->z * s;
    v2->w = v->w * s;

    return v2;
}

vec4 *vec4_div_scalar(vec4* const v, float const s) {
    vec4* v2 = (vec4 *)calloc(1, sizeof(vec4));

    v2->x = v->x / s;
    v2->y = v->y / s ;
    v2->z = v->z / s;
    v2->w = v->w / s;

    return v2;
}


vec4 *vec4_normalize(vec4* const v) {
    vec4 *v2  = (vec4 *)calloc(1, sizeof(vec4));
    float v_len = vec4_length(v);

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;
    v2->z = v->z / v_len;
    v2->w = v->w / v_len;

    return v2;
}

void vec4_print(vec4* const v) {
    printf("x:%.2f y:%.2f z%.2f w%.2f\n",
           v->x,
           v->y,
           v->z,
           v->w);
}
