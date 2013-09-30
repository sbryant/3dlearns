#include "clar.h"

#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>

#include "matrix.h"
#include "vector.h"

void test_matrix__initialize(void) {

}

void test_matrix__cleanup(void) {

}

void test_matrix__ident(void) {
    mat4x4 m4;

    mat4x4_make_ident(&m4);
    mat4x4 ident = MAT4_IDENT;

    cl_assert_(m4.x[0] == ident.x[0], "X vector first element must be 1.0");
    cl_assert_(m4.y[1] == ident.y[1], "Y vector second element must be 1.0");
    cl_assert_(m4.z[2] == ident.z[2], "Z vector third element must be 1.0");
    cl_assert_(m4.w[3] == ident.w[3], "W vector fourth element must be 1.0");
}

void test_matrix__vector_mul(void) {
    mat4x4 m4;

    mat4x4_make_ident(&m4);

    m4.y[1] = 2.0;

    vec4x1 v = { 1.0, 1.0, 1.0, 1.0 };
    vec4x1 *r = mat4x4_mul_vec4x1(&m4, &v);

    /* assert a scale */
    cl_assert_(r->x == 1.0 && r->y == 2.0 && r->z == 1.0 && r->w == 1.0, "Didn't scale Y by 2");
    free(r);
}


void test_matrix__matrix_mul(void) {
    mat4x4 m, m2;

    mat4x4_make_ident(&m);
    mat4x4_make_ident(&m2);

    mat4x4 *r = mat4x4_mul(&m, &m2);

    cl_assert_(m.x[0] == r->x[0], "X vector first element must be 1.0");
    cl_assert_(m.y[1] == r->y[1], "Y vector second element must be 1.0");
    cl_assert_(m.z[2] == r->z[2], "Z vector third element must be 1.0");
    cl_assert_(m.w[3] == r->w[3], "W vector fourth element must be 1.0");


    r->x[0] = 2.0;
    r->y[1] = 2.0;
    r->z[2] = 2.0;
    r->w[3] = 1.0;

    mat4x4 *r2 = mat4x4_mul(&m, r);

    cl_assert_(r2->x[0] == 2.0, "X vector first element must be 2.0");
    cl_assert_(r2->y[1] == 2.0, "Y vector second element must be 2.0");
    cl_assert_(r2->z[2] == 2.0, "Z vector third element must be 2.0");
    cl_assert_(r2->w[3] == 1.0, "W vector fourth element must be 1.0");


    mat4x4 *r3 = mat4x4_mul(r2, r2);
    cl_assert_(r3->x[0] == 4.0, "X vector first element must be 4.0");
    cl_assert_(r3->y[1] == 4.0, "Y vector second element must be 4.0");
    cl_assert_(r3->z[2] == 4.0, "Z vector third element must be 4.0");
    cl_assert_(r3->w[3] == 1.0, "W vector fourth element must be 1.0");

    free(r);
    free(r2);
    free(r3);
}
