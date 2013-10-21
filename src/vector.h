#pragma once
typedef struct s_vec4 {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct s_vec3 {
    float x;
    float y;
    float z;
} vec3;

float vec4_length(vec4* const v);
float vec3_length(vec3* const v);
float vec4_length_square(vec4* const v);

vec4* vec4_add(vec4* const v1, vec4* const v2);
vec3* vec3_add(vec3* const v1, vec3* const v2);
vec4* vec4_sub(vec4* const v1, vec4* const v2);
vec3* vec3_sub(vec3* const v1, vec3* const v2);

vec4 *vec4_mul_scalar(vec4* const v, float const s);
vec3 *vec3_mul_scalar(vec3* const v, float const s);
vec4 *vec4_div_scalar(vec4* const v, float const s);

vec4 *vec4_normalize(vec4* const v);
vec3 *vec3_normalize(vec3* const v);

float vec4_dot(vec4* const a, vec4* const b);
float vec3_dot(vec3* const a, vec3* const b);
vec4* vec4_cross(vec4* const a, vec4* const b);
vec3* vec3_cross(vec3* const a, vec3* const b);

vec4* vec4_make(void);
vec3* vec3_make(void);

void vec3_set_vec3(vec3 *v1, vec3 *v2);

void vec4_print(vec4* const v);
