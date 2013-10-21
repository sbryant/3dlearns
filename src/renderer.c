#include "renderer.h"
#include "utils.h"
#include <GL/glew.h>

app* application = NULL;

renderer* make_renderer(int w, int h) {
    renderer* r = (renderer*)calloc(1, sizeof(renderer));
    renderer_set_size(r, w, h);

    r->camera_pos = vec3_make();
    r->camera_dir = vec3_make();
    r->camera_up  = vec3_make();

    glGenBuffers(1, &(r->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);

    /* XYZ float attributes */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return r;
}

void renderer_initialize(renderer* r) {

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

void renderer_set_camera_up(renderer* r, vec3* up) {
    r->camera_up->x = up->x;
    r->camera_up->y = up->y;
    r->camera_up->z = up->z;
}

void renderer_set_camera_dir(renderer* r, vec3* dir) {
    r->camera_dir->x = dir->x;
    r->camera_dir->y = dir->y;
    r->camera_dir->z = dir->z;
}

void renderer_set_camera_position(renderer* r, vec3* pos) {
    r->camera_pos->x = pos->x;
    r->camera_pos->y = pos->y;
    r->camera_pos->z = pos->z;
}

vec3 *renderer_camera_position(renderer *r) {
    return r->camera_pos;
}

render_context* make_render_context(void) {
    render_context* r = (render_context*)calloc(1, sizeof(render_context));
    r->proj = mat4x4_make_ident(NULL);
    r->view = mat4x4_make_ident(NULL);
    r->model = mat4x4_make_ident(NULL);

    return r;
}

rendering_context* make_rendering_context(renderer* rndr) {
    rendering_context* r = (rendering_context*)calloc(1, sizeof(rendering_context));
    render_context* rc = make_render_context();

    r->context = rc;
    r->contexts = (render_context**)calloc(RC_MAX_CONTEXTS, sizeof(render_context*));
    r->contexts[0] = rc;
    r->num_render_contexts = 1;

    r->shader = rndr->shader;
    r->program = r->shader->program;
    r->vbo = rndr->vbo;

    r->renderer = rndr;


    return r;
}

mat4x4* rendering_context_projection(rendering_context *r) {
    return r->context->proj;
}

mat4x4* rendering_context_view(rendering_context *r) {
    return r->context->view;
}

void rendering_context_set_projection(rendering_context *r, const mat4x4 *m) {
    mat4x4_set_mat4x4(r->context->proj, m);
}

void rendering_context_set_view(rendering_context *r, const mat4x4 *m) {
    mat4x4_set_mat4x4(r->context->view, m);
}

void renderer_start_rendering(renderer *rndr, rendering_context *rc) {
    rndr->width = application->w;
    rndr->height = application->h;

    float aspect = rndr->width/rndr->height;

    mat4x4 *ident = mat4x4_make_ident(NULL);

    mat4x4 *proj = perspective(rndr->camera_fov, aspect, rndr->camera_near, rndr->camera_far);
    rendering_context_set_projection(rc, proj);

    mat4x4 *view = look_at(rndr->camera_pos, rndr->camera_dir, rndr->camera_up);
    rendering_context_set_view(rc, view);

    mat4x4_cleanup(view);
    mat4x4_cleanup(ident);
    mat4x4_cleanup(proj);

    glViewport(0, 0, rndr->width, rndr->height);

    glEnable(GL_MULTISAMPLE);
}

void rendering_context_end_render(rendering_context *r) {
    glUseProgram(r->shader->program);

    render_context *context = r->context;
    rendering_context_set_uniform_mat4x4(r, "proj", context->proj);
    rendering_context_set_uniform_mat4x4(r, "view", context->view);
    rendering_context_set_uniform_mat4x4(r, "model", context->model);

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * r->num_verts, r->verts);

    glEnableVertexAttribArray(r->shader->pos_attr);
    glVertexAttribPointer(r->shader->pos_attr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(r->draw_mode, 0, r->num_verts);
    glDisableVertexAttribArray(r->shader->pos_attr);
    glDisableVertexAttribArray(r->shader->color_attr);
}

void rendering_context_begin_render_tri_fan(rendering_context *r) {
    r->draw_mode = GL_TRIANGLE_FAN;
}

void rendering_context_render_tri_face(rendering_context *r, float *face, int size) {
    r->verts = face;
    r->num_verts = size;
}

void rendering_context_render_box(rendering_context* rc, vec3 *min, vec3 *max) {
    vec3 forward = { max->x - min->x, 0.0, 0.0 };
    vec3 up = { 0.0, max->y - min->y, 0.0 };
    vec3 right = { 0.0, 0.0, max->z - min->z };

    vec3 *mr = vec3_add(min, &right);
    vec3 *mru = vec3_add(mr, &up);
    vec3 *mu = vec3_add(min, &up);
    vec3 *muf = vec3_add(mu, &forward);
    vec3 *mf = vec3_add(min, &forward);
    vec3 *mfr = vec3_add(mf, &right);
    vec3 *uf = vec3_add(&up, &forward);
    vec3 *mruf = vec3_add(mr, uf);
    vec3 *mfu = vec3_add(mf, &up);

    float back_verts[] = {
        /* back face */
        min->x, min->y, min->z,  /* lower left */
        mr->x, mr->y, mr->z, /* lower right */
        mru->x, mru->y, mru->z, /* top right */
        mu->x, mu->y, mu->z /* top left */
    };

    float left_verts[] = {
        /* left face */
        min->x, min->y, min->z,
        mu->x, mu->y, mu->z,
        muf->x, muf->y, muf->z,
        mf->x, mf->y, mf->z
    };

    float bottom_verts[] = {
        /* bottom face */
        min->x, min->y, min->z,
        mf->x, mf->y, mf->z,
        mfr->x, mfr->y, mfr->z,
        mr->x, mr->y, mr->z
    };

    float top_verts[] = {
        /* top face */
        mruf->x, mruf->y, mruf->z,
        muf->x, muf->y, muf->z,
        mu->x, mu->y, mu->z,
        mru->x, mru->y, mru->z
    };

    float right_verts[] = {
        /* right face */
        mruf->x, mruf->y, mruf->z,
        mru->x, mru->y, mru->z,
        mr->x, mr->y, mr->z,
        mfr->x, mfr->y, mfr->z
    };

    float front_verts[] = {
        /* front face */
        mruf->x, mruf->y, mruf->z,
        mfr->x, mfr->y, mfr->z,
        mf->x, mf->y, mf->z,
        mfu->x, mfu->y, mfu->z
    };

//    render_context *r = rc->context;

    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, back_verts, sizeof(back_verts) / sizeof(float));
    rendering_context_end_render(rc);

    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, left_verts, sizeof(left_verts) / sizeof(float));
    rendering_context_end_render(rc);

    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, bottom_verts, sizeof(bottom_verts) / sizeof(float));
    rendering_context_end_render(rc);

    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, top_verts, sizeof(top_verts) / sizeof(float));
    rendering_context_end_render(rc);

    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, right_verts, sizeof(right_verts) / sizeof(float));
    rendering_context_end_render(rc);

    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, front_verts, sizeof(front_verts) / sizeof(float));
    rendering_context_end_render(rc);

}

void rendering_context_set_uniform_vec4(rendering_context* rc, const char* name, vec4 *v) {
    int uniform = glGetUniformLocation(rc->program, name);
    float vec[] = { v->x, v->y, v->z, v->w };
    glUniform4fv(uniform, 1, vec);
}

void rendering_context_set_uniform_mat4x4(rendering_context* rc, const char* name, mat4x4 *m) {
    int uniform = glGetUniformLocation(rc->program, name);
    glUniformMatrix4fv(uniform, 1, false, m->m);
}

void rendering_context_set_uniform_4vf(rendering_context* rc, const char* name, float* m) {
    int uniform = glGetUniformLocation(rc->program, name);
    glUniform4fv(uniform, 1, m);
}
