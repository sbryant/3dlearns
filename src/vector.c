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

float vec3_length(const vec3* v) {
    return sqrtf((v->x * v->x) +
                 (v->y * v->y) +
                 (v->z * v->z));
}

float vec4_length_square(vec4* const v) {
    return (v->x * v->x) +
        (v->y * v->y) +
        (v->z * v->z) +
        (v->w * v->w);
}

vec4* vec4_add(vec4* const v1, vec4* const v2) {
    vec4 *vr = vec4_make();

    vr->x = v1->x + v2->x;
    vr->y = v1->y + v2->y;
    vr->z = v1->z + v2->z;
    vr->w = v1->w + v2->w;

    return vr;
}

vec3* vec3_add(const vec3 *v1, const vec3 *v2, vec3 *out) {
    vec3 *vr = out;
    if (vr == NULL)
        vr = vec3_make();

    vr->x = v1->x + v2->x;
    vr->y = v1->y + v2->y;
    vr->z = v1->z + v2->z;

    return vr;
}

vec4* vec4_sub(vec4* const v1, vec4* const v2) {
    vec4 *vr = vec4_make();

    vr->x = v1->x - v2->x;
    vr->y = v1->y - v2->y;
    vr->z = v1->z - v2->z;
    vr->w = v1->w - v2->w;

    return vr;
}

vec3* vec3_sub(const vec3 *v1, const vec3 *v2, vec3 *out) {
    vec3 *vr = out;
    if (vr == NULL)
        vr = vec3_make();

    vr->x = v1->x - v2->x;
    vr->y = v1->y - v2->y;
    vr->z = v1->z - v2->z;

    return vr;
}

/* Vec4 scaling functions */

vec4 *vec4_mul_scalar(vec4* const v, float const s) {
    vec4* v2 = vec4_make();

    v2->x = v->x * s;
    v2->y = v->y * s;
    v2->z = v->z * s;
    v2->w = v->w * s;

    return v2;
}

vec3 *vec3_mul_scalar(const vec3* v, const float s, vec3 *out) {
    vec3* v2 = out;
    if (v2 == NULL)
        v2 = vec3_make();

    v2->x = v->x * s;
    v2->y = v->y * s;
    v2->z = v->z * s;

    return v2;
}

vec4 *vec4_div_scalar(vec4* const v, float const s) {
    vec4* v2 = vec4_make();

    v2->x = s / v->x ;
    v2->y = s / v->y;
    v2->z = s / v->z;
    v2->w = s / v->w;

    return v2;
}

vec4* vec4_make(void) {
    return (vec4 *)calloc(1, sizeof(vec4));
}

vec3* vec3_make(void) {
    return (vec3 *)calloc(1, sizeof(vec3));
}


vec4 *vec4_normalize(vec4* const v) {
    vec4 *v2  = vec4_make();
    float v_len = vec4_length(v);

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;
    v2->z = v->z / v_len;
    v2->w = v->w / v_len;

    return v2;
}

vec3 *vec3_normalize(const vec3 *v, vec3 *out) {
    vec3 *v2  = out;
    if (v2 == NULL)
        v2 = vec3_make();

    float v_len = vec3_length(v);

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;
    v2->z = v->z / v_len;

    return v2;
}

float vec4_dot(vec4* const a, vec4* const b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z) + (a->w * b->w);
}

float vec3_dot(vec3* const a, vec3* const b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

vec4* vec4_cross(vec4* const a, vec4* const b) {
    vec4 *v  = vec4_make();
    v->x = (a->y * b->z) - (b->y * a->z);
    v->y = (a->z * b->x) - (b->z * a->x);
    v->z = (a->x * b->y) - (b->x * a->y);
    v->w = 1.0;

    return v;
}

vec3* vec3_cross(const vec3* a, const vec3* b, vec3 *out) {
    vec3 *v  = out;
    if (v == NULL)
        v = vec3_make();

    v->x = (a->y * b->z) - (a->z * b->y);
    v->y = (a->z * b->x) - (a->x * b->z);
    v->z = (a->x * b->y) - (a->y * b->x);

    return v;
}

void vec4_print(vec4* const v) {
    printf("x:%.2f y:%.2f z%.2f w%.2f\n",
           v->x,
           v->y,
           v->z,
           v->w);
}


void vec3_set_vec3(vec3 *v1, vec3 *v2) {
    v1->x = v2->x;
    v1->y = v2->y;
    v1->z = v2->z;
}
