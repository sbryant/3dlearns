#pragma once
#include <SDL.h>
#include "matrix.h"
#include "vector.h"


#define GLPG_SHADER_READ_SIZE 1024

char *read_shader(const char* path);

mat4x4* look_at(vec3 *pos, vec3 *center, vec3 *up);
mat4x4* perspective(const float fovy, const float aspect, const float znear, const float zfar);
void get_screen_size(int *w, int *h);
float approach(float goal, float current, float dt);
