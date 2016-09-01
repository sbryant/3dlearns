#pragma once
#if !defined(_WIN32)
#include <unistd.h>
#endif
#include <SDL.h>
#include "matrix.h"
#include "vector.h"

void get_screen_size(int *w, int *h);
float approach(float goal, float current, float dt);
