#pragma once
#include <SDL.h>
#include "matrix.h"
#include "vector.h"


#define GLPG_SHADER_READ_SIZE 1024

char *read_shader(const char* path);
void get_screen_size(int *w, int *h);
float approach(float goal, float current, float dt);
