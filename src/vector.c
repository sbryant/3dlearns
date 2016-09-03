#include "vector.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

float vec4_length(const vec4* v) {
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

float vec4_length_square(const vec4* v) {
    return (v->x * v->x) +
        (v->y * v->y) +
        (v->z * v->z) +
        (v->w * v->w);
}

vec4* vec4_add(const vec4* v1, const vec4* v2) {
    vec4 *vr = vec4_make();

    vr->x = v1->x + v2->x;
    vr->y = v1->y + v2->y;
    vr->z = v1->z + v2->z;
    vr->w = v1->w + v2->w;

    return vr;
}

void vec3_add(const vec3 *v1, const vec3 *v2, vec3 *out) {
    vec3 *vr = out;

    vr->x = v1->x + v2->x;
    vr->y = v1->y + v2->y;
    vr->z = v1->z + v2->z;
}

vec4* vec4_sub(const vec4* v1, const vec4* v2) {
    vec4 *vr = vec4_make();

    vr->x = v1->x - v2->x;
    vr->y = v1->y - v2->y;
    vr->z = v1->z - v2->z;
    vr->w = v1->w - v2->w;

    return vr;
}

void vec3_sub(const vec3 *v1, const vec3 *v2, vec3 *out) {
    vec3 *vr = out;

    vr->x = v1->x - v2->x;
    vr->y = v1->y - v2->y;
    vr->z = v1->z - v2->z;
}

/* Vec4 scaling functions */

vec4* vec4_mul_scalar(const vec4* v, float s) {
    vec4* v2 = vec4_make();

    v2->x = v->x * s;
    v2->y = v->y * s;
    v2->z = v->z * s;
    v2->w = v->w * s;

    return v2;
}

void vec3_mul_scalar(const vec3* v, float s, vec3* out) {
    vec3* v2 = out;

    v2->x = v->x * s;
    v2->y = v->y * s;
    v2->z = v->z * s;
}

vec4 *vec4_div_scalar(const vec4*  v, float s) {
    vec4* v2 = vec4_make();

    if(!v2)
        return v2;

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


vec4 *vec4_normalize(const vec4* v) {
    vec4 *v2  = vec4_make();
    float v_len = vec4_length(v);

    if(!v_len)
        return v2;

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;
    v2->z = v->z / v_len;
    v2->w = v->w / v_len;

    return v2;
}

void vec3_normalize(const vec3 *v, vec3 *out) {
    vec3 *v2  = out;

    float v_len = vec3_length(v);

    if(!v_len)
        return v2;

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;
    v2->z = v->z / v_len;

    return v2;
}

float vec4_dot(const vec4* a, const vec4* b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z) + (a->w * b->w);
}

float vec3_dot(const vec3* a, const vec3* b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

vec4* vec4_cross(const vec4* a, const vec4* b) {
    vec4 *v  = vec4_make();
    v->x = (a->y * b->z) - (b->y * a->z);
    v->y = (a->z * b->x) - (b->z * a->x);
    v->z = (a->x * b->y) - (b->x * a->y);
    v->w = 1.0;

    return v;
}

void vec3_cross(const vec3* a, const vec3* b, vec3 *out) {
    vec3 *v  = out;
    v->x = (a->y * b->z) - (a->z * b->y);
    v->y = (a->z * b->x) - (a->x * b->z);
    v->z = (a->x * b->y) - (a->y * b->x);
}

void vec4_print(const vec4* v) {
    printf("x:%.2f y:%.2f z%.2f w%.2f\n",
           v->x,
           v->y,
           v->z,
           v->w);
}

void vec3_print(const vec3* v) {
    printf("x:%.5f y:%.5f z%.5f",
           v->x,
           v->y,
           v->z);
}


void vec3_copy(vec3 *dest, vec3 *src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}
