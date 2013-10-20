#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

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

mat4x4* look_at(vec3 *pos, vec3 *center, vec3 *up) {
    mat4x4* p = mat4x4_make_ident(NULL);

    vec3* front = vec3_sub(center, pos);

    vec3* f = vec3_normalize(front);
    vec3* side = vec3_cross(f, up);

    vec3* s = vec3_normalize(side);
    vec3* u = vec3_cross(s, f);

    p->x[0] = s->x;
    p->y[0] = s->y;
    p->z[0] = s->z;

    p->x[1] = u->x;
    p->y[1] = u->y;
    p->z[1] = u->z;

    p->x[2] = -f->x;
    p->y[2] = -f->y;
    p->z[2] = -f->z;

    mat4x4* res = mat4x4_translate(p, -pos->x, -pos->y, -pos->z);

    mat4x4_cleanup(p);
    free(front);
    free(side);
    free(s);
    free(u);
    free(f);

    return res;
}


mat4x4* perspective(const float fovy, const float aspect, const float znear, const float zfar) {
    assert(aspect != 0.0);
    assert(znear != zfar);

    float f = 1.0 / tan(fovy / 2.0);

    mat4x4 *res = mat4x4_make_ident(NULL);
    res->x[0] = f / aspect;
    res->y[1] = f;
    res->z[2] = (zfar+znear) / (znear-zfar);
    res->z[3] = -1.0;
    res->w[2] = (2 * zfar * znear) / (znear-zfar);
    res->w[3] = 0.0;

    return res;
}
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
