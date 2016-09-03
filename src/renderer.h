#pragma once
#include <stdlib.h> /* size_t */
#include <assert.h>

#include "linmath.h"
#include "shader.h"

#define RC_MAX_CONTEXTS 1

typedef struct s_app {
    int w;
    int h;
} app;

typedef struct s_renderer {
    int height, width;
    vec3 camera_pos;
    vec3 camera_dir;
    vec3 camera_up;
    float camera_fov, camera_near, camera_far;

    shader *shader;
    unsigned int vbo;
} renderer;


typedef struct s_render_context {
    mat4x4 proj;
    mat4x4 view;
    mat4x4 model;

    int viewport_x;
    int viewport_y;
    int viewport_width;
    int viewport_height;

} render_context;

typedef struct s_rendering_context {
    size_t num_render_contexts;
    render_context *context;

    int num_verts;
    float *verts;

    renderer *renderer;

    shader *shader;
    int draw_mode;
    int program;
    unsigned int vbo;
} rendering_context;

renderer* make_renderer(int w, int h);
void renderer_init(renderer* r);
void renderer_set_size(renderer *r, int w, int h);
void renderer_set_camera_fov(renderer* r, float fov);
void renderer_set_camera_near(renderer* r, float near);
void renderer_set_camera_far(renderer* r, float far);
void renderer_set_camera_up(renderer* r, const vec3 up);
void renderer_set_camera_dir(renderer* r, const vec3 dir);
void renderer_set_camera_position(renderer* r, const vec3 pos);

render_context* make_render_context(void);

rendering_context* make_rendering_context(renderer* rndr, unsigned int width, unsigned int height);
void rendering_context_set_projection(rendering_context *r, mat4x4 m);
void rendering_context_set_view(rendering_context *r, mat4x4 m);
void renderer_start_rendering(renderer *rndr, rendering_context *rc, unsigned int width, unsigned int height);
void renderer_finish_rendering(renderer *rndr, rendering_context *rc);
void rendering_context_set_uniform_vec4(rendering_context* rc, const char* name, vec4 v);
void rendering_context_set_uniform_mat4x4(rendering_context* rc, const char* name, const mat4x4 m);
void rendering_context_set_uniform_4vf(rendering_context* rc, const char* name, float m);
void rendering_context_end_render(rendering_context *r);
