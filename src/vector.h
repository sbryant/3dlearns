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

float vec4_length(const vec4* v);
float vec3_length(const vec3* v);
float vec4_length_square(const vec4* v);

vec4* vec4_add(const vec4* v1, const vec4* v2);
void vec3_add(const vec3 *v1, const vec3 *v2, vec3 *out);
vec4* vec4_sub(const vec4* v1, const vec4* v2);
void vec3_sub(const vec3 *v1, const vec3 *v2, vec3 *out);

vec4 *vec4_mul_scalar(const vec4* v, float s);
void vec3_mul_scalar(const vec3 *v, float s, vec3* out);
vec4 *vec4_div_scalar(const vec4* v, float s);

vec4 *vec4_normalize(const vec4* v);
void vec3_normalize(const vec3 *v, vec3 *out);

float vec4_dot(const vec4* a, const vec4* b);
float vec3_dot(const vec3* a, const vec3* b);
vec4* vec4_cross(const vec4* a, const vec4* b);
void vec3_cross(const vec3 *a, const vec3 *b, vec3 *out);

vec4* vec4_make(void);
vec3* vec3_make(void);

void vec3_copy(vec3 *v1, vec3 *v2);

void vec4_print(const vec4* v);
void vec3_print(const vec3* v);
