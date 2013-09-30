#pragma once
typedef struct s_vec4x1 {
    float x;
    float y;
    float z;
    float w;
} vec4x1;

float vec4x1_length(vec4x1* const v);
float vec4x1_length_square(vec4x1* const v);

vec4x1* vec4x1_add(vec4x1* const v1, vec4x1* const v2);
vec4x1* vec4x1_sub(vec4x1* const v1, vec4x1* const v2);

vec4x1 *vec4x1_mul_scalar(vec4x1* const v, float const s);
vec4x1 *vec4x1_div_scalar(vec4x1* const v, float const s);

vec4x1 *vec4x1_normalize(vec4x1* const v);

void vec4x1_print(vec4x1* const v);
