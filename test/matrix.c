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

void test_matrix__mul(void) {
    mat4x4 m4;

    mat4x4_make_ident(&m4);

    m4.y[1] = 2.0;

    vec4x1 v = { 1.0, 1.0, 1.0, 1.0 };
    vec4x1 *r = mat4x4_mul_vec4x1(&m4, &v);

    /* assert a scale */
    cl_assert_(r->x == 1.0 && r->y == 2.0 && r->z == 1.0 && r->w == 1.0, "Didn't scale Y by 2");
    free(r);
}
