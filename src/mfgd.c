#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct s_vector {
    float x;
    float y;
} vector;

typedef struct s_point {
    float x;
    float y;
} point;


point* point_add_vector(point* const p, vector* const v) {
    point *p2 = (point*)calloc(1, sizeof(point));

    p2->x = p->x + v->x;
    p2->y = p->y + v->y;

    return p2;
}

vector* point_sub(point* const a, point* const b) {
    vector* v = (vector*)calloc(1, sizeof(vector));
    v->x = a->x - b->x;
    v->y = a->y - b->y;

    return v;
}


float vector_length(vector* const v) {
    return sqrtf((v->x * v->x) + (v->y * v->y));
}

float vector_length_square(vector* const v) {
    return (v->x * v->x) + (v->y * v->y);
}

/* Vector scaling functions */

vector *vector_mul_scalar(vector* const v, float const s) {
    vector* v2 = (vector *)calloc(1, sizeof(vector));

    v2->x = v->x * s;
    v2->y = v->y * s ;

    return v2;
}

vector *vector_div_scalar(vector* const v, float const s) {
    vector* v2 = (vector *)calloc(1, sizeof(vector));

    v2->x = v->x / s;
    v2->y = v->y / s ;

    return v2;
}


vector *vector_normalize(vector* const v) {
    vector *v2  = (vector *)calloc(1, sizeof(vector));
    float v_len = vector_length(v);

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;

    return v2;
}

int main(int argc, char** argv) {

    vector v = { 3.0, 4.0 };
    printf("Initial speed: %.2f\n", vector_length(&v));

    vector *d = vector_mul_scalar(&v, 2.0);
    printf("Doubled speed: %.2f\n", vector_length(d));

    vector *h = vector_div_scalar(&v, 2.0);
    printf("Halved speed: %.2f\n", vector_length(h));

    return 0;
}
