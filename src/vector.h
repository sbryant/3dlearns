#pragma once
typedef struct s_vector {
    float x;
    float y;
} vector;

typedef struct s_point {
    float x;
    float y;
} point;


point* point_add_vector(point* const p, vector* const v);
vector* point_sub(point* const a, point* const b);

float vector_length(vector* const v);
float vector_length_square(vector* const v);

vector* vector_add(vector* const v1, vector* const v2);
vector* vector_sub(vector* const v1, vector* const v2);

vector *vector_mul_scalar(vector* const v, float const s);
vector *vector_div_scalar(vector* const v, float const s);

vector *vector_normalize(vector* const v);
