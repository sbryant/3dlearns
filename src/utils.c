#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "utils.h"

char *read_shader(const char* path) {
	int fd = open(path, O_RDONLY);
	char buffer[GLPG_SHADER_READ_SIZE];
	char *result = NULL;
	size_t file_size = 0;
	size_t bytes_read = 0;
	size_t offset = 0;

	while((bytes_read = read(fd, &buffer, GLPG_SHADER_READ_SIZE))) {
		if(bytes_read == -1)
			return NULL;

		offset = file_size;
		file_size += bytes_read;
		result = (char*)realloc(result, file_size + 1);
		memcpy(result + offset, &buffer, bytes_read);
		result[file_size] = '\0';
	}

	close(fd);

	return result;
};

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
