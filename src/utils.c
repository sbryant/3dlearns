#include "utils.h"

#if !defined(_WIN32)
#include <unistd.h>
#endif
#include <SDL.h>

float approach(float goal, float current, float dt) {
	float diff = goal - current;

	if (diff > dt)
		return current + dt;

	if (diff < -dt)
		return current - dt;

	return goal;
}

void get_screen_size(int *w, int *h) {
	SDL_DisplayMode info;

	SDL_GetCurrentDisplayMode(0, &info);
	*w = info.w;
	*h = info.h;
}
