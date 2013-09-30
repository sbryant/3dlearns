#include "vector.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

float vec4x1_length(vec4x1* const v) {
    return sqrtf((v->x * v->x) +
                 (v->y * v->y) +
                 (v->z * v->z) +
                 (v->w * v->w));
}

float vec4x1_length_square(vec4x1* const v) {
    return (v->x * v->x) +
        (v->y * v->y) +
        (v->z * v->z) +
        (v->w * v->w);
}

vec4x1* vec4x1_add(vec4x1* const v1, vec4x1* const v2) {
    vec4x1 *vr = (vec4x1*)calloc(1, sizeof(vec4x1));

    vr->x = v1->x + v2->x;
    vr->y = v1->y + v2->y;
    vr->z = v1->z + v2->z;
    vr->w = v1->w + v2->w;

    return vr;
}

vec4x1* vec4x1_sub(vec4x1* const v1, vec4x1* const v2) {
    vec4x1 *vr = (vec4x1*)calloc(1, sizeof(vec4x1));

    vr->x = v1->x - v2->x;
    vr->y = v1->y - v2->y;
    vr->z = v1->z - v2->z;
    vr->w = v1->w - v2->w;

    return vr;
}

/* Vec4x1 scaling functions */

vec4x1 *vec4x1_mul_scalar(vec4x1* const v, float const s) {
    vec4x1* v2 = (vec4x1 *)calloc(1, sizeof(vec4x1));

    v2->x = v->x * s;
    v2->y = v->y * s;
    v2->z = v->z * s;
    v2->w = v->w * s;

    return v2;
}

vec4x1 *vec4x1_div_scalar(vec4x1* const v, float const s) {
    vec4x1* v2 = (vec4x1 *)calloc(1, sizeof(vec4x1));

    v2->x = v->x / s;
    v2->y = v->y / s ;
    v2->z = v->z / s;
    v2->w = v->w / s;

    return v2;
}


vec4x1 *vec4x1_normalize(vec4x1* const v) {
    vec4x1 *v2  = (vec4x1 *)calloc(1, sizeof(vec4x1));
    float v_len = vec4x1_length(v);

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;
    v2->z = v->z / v_len;
    v2->w = v->w / v_len;

    return v2;
}

void vec4x1_print(vec4x1* const v) {
    printf("x:%.2f y:%.2f z%.2f w%.2f\n",
           v->x,
           v->y,
           v->z,
           v->w);
}
