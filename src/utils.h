#pragma once
#include <unistd.h>
#include <SDL.h>
#include "matrix.h"
#include "vector.h"

void get_screen_size(int *w, int *h);
float approach(float goal, float current, float dt);
