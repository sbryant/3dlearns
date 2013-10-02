#pragma once
typedef struct s_vec4 {
    float x;
    float y;
    float z;
    float w;
} vec4;

float vec4_length(vec4* const v);
float vec4_length_square(vec4* const v);

vec4* vec4_add(vec4* const v1, vec4* const v2);
vec4* vec4_sub(vec4* const v1, vec4* const v2);

vec4 *vec4_mul_scalar(vec4* const v, float const s);
vec4 *vec4_div_scalar(vec4* const v, float const s);

vec4 *vec4_normalize(vec4* const v);

void vec4_print(vec4* const v);
