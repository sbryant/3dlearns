#include "renderer.h"
#include "utils.h"
#include <glew.h>
#include <string.h>

void renderer_init(renderer* r) {
	glGenBuffers(1, &(r->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1024, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderer_set_size(renderer *r, int w, int h) {
    r->height = h;
    r->width = w;
}

void renderer_set_camera_fov(renderer* r, float fov) {
    r->camera_fov = fov;
}

void renderer_set_camera_near(renderer* r, float near) {
    r->camera_near = near;
}

void renderer_set_camera_far(renderer* r, float far) {
    r->camera_far = far;
}

void renderer_set_camera_up(renderer* r, const vec3 up) {
	r->camera_up[0] = up[0];
    r->camera_up[1] = up[1];
    r->camera_up[2] = up[2];
}

void renderer_set_camera_dir(renderer* r, const vec3 dir) {
    r->camera_dir[0] = dir[0];
    r->camera_dir[1] = dir[1];
    r->camera_dir[2] = dir[2];
}

void renderer_set_camera_position(renderer* r, const vec3 pos) {
    r->camera_pos[0] = pos[0];
    r->camera_pos[1] = pos[1];
    r->camera_pos[2] = pos[2];
}

render_context* make_render_context(void) {
    render_context* r = (render_context*)calloc(1, sizeof(*r));

	mat4x4_identity(r->proj);
	mat4x4_identity(r->view);
	mat4x4_identity(r->model);

    return r;
}

rendering_context* make_rendering_context(renderer* rndr, unsigned int width, unsigned int height) {
	assert(width > 0 && height > 0);

    rendering_context* r = (rendering_context*)calloc(1, sizeof(*r));
    render_context* rc = make_render_context();

    r->context = rc;

    r->shader = rndr->shader;
    r->program = r->shader->program;
    r->vbo = rndr->vbo;

    r->renderer = rndr;

    r->context->viewport_width = width;
    r->context->viewport_height = height;
    r->context->viewport_x = 0;
    r->context->viewport_y = 0;

    return r;
}

void rendering_context_set_projection(rendering_context *r, mat4x4 m) {
	mat4x4_dup(r->context->proj, m);
}

void rendering_context_set_view(rendering_context *r, mat4x4 m) {
	mat4x4_dup(r->context->view, m);
}

void renderer_start_rendering(renderer *rndr, rendering_context *rc, unsigned int width, unsigned int height) {
    assert(width > 0 && height > 0);

	rndr->width = width;
	rndr->height = height;

    float aspect = rndr->width/(float)rndr->height;

	mat4x4_perspective(rc->context->proj, rndr->camera_fov, aspect, rndr->camera_near, rndr->camera_far);
  
	mat4x4 view;
	mat4x4_look_at(view, rndr->camera_pos, rndr->camera_dir, rndr->camera_up);

    rendering_context_set_view(rc, view);

    glViewport(0, 0, rndr->width, rndr->height);

    glEnable(GL_MULTISAMPLE);
}

void renderer_finish_rendering(renderer *rndr, rendering_context *rc) {
}

void rendering_context_end_render(rendering_context *r) {
    render_context *context = r->context;
    rendering_context_set_uniform_mat4x4(r, "proj", context->proj);
    rendering_context_set_uniform_mat4x4(r, "view", context->view);
    rendering_context_set_uniform_mat4x4(r, "model", context->model);

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * r->num_verts * 6, r->verts);

    glEnableVertexAttribArray(r->shader->pos_attr);
    glVertexAttribPointer(r->shader->pos_attr, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

    glEnableVertexAttribArray(r->shader->color_attr);
    glVertexAttribPointer(r->shader->color_attr, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
    glDrawArrays(r->draw_mode, 0, r->num_verts);

    glDisableVertexAttribArray(r->shader->pos_attr);
    glDisableVertexAttribArray(r->shader->color_attr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void rendering_context_set_uniform_vec4(rendering_context* rc, const char* name, vec4 v) {
    int uniform = glGetUniformLocation(rc->program, name);
    glUniform4fv(uniform, 1, v);
}

void rendering_context_set_uniform_mat4x4(rendering_context* rc, const char* name, const mat4x4 m) {
    int uniform = glGetUniformLocation(rc->program, name);
    glUniformMatrix4fv(uniform, 1, 0, (const GLfloat *)m);
}

void rendering_context_set_uniform_4vf(rendering_context* rc, const char* name, float m) {
    int uniform = glGetUniformLocation(rc->program, name);
    glUniform4fv(uniform, 1, &m);
}
