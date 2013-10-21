#include <GL/glew.h>
#include <stdint.h>
#include <SDL.h>

#include "matrix.h"
#include "vector.h"
#include "utils.h"
#include "renderer.h"
#include "shader.h"

typedef struct s_character {
    vec3 pos;
    vec3 velocity;
    vec3 velocity_goal;
    vec3 gravity;
} character;

character box;

void update(float dt) {
    box.velocity.x = approach(box.velocity_goal.x, box.velocity.x, dt * 65.0f);
    box.velocity.z = approach(box.velocity_goal.z, box.velocity.z, dt * 65.0f);

    vec3 *new_vel = vec3_mul_scalar(&(box.velocity), dt);
    vec3 *new_pos = vec3_add(&(box.pos), new_vel);

    vec3_set_vec3(&(box.pos), new_pos);
    free(new_vel); free(new_pos);

    new_vel = NULL;

    vec3 *new_grav = vec3_mul_scalar(&(box.gravity), dt);
    new_vel = vec3_add(&(box.velocity), new_grav);
    vec3_set_vec3(&(box.velocity), new_vel);
    free(new_grav); free(new_vel);

    if (box.pos.y < 0)
        box.pos.y = 0.0f;
}

void draw(renderer *rndr) {
    vec3 shift = { 0.0, 4.0, -6.0 };
    vec3 *camera_pos = vec3_add(&(box.pos), &shift);
    renderer_set_camera_position(rndr, camera_pos);
    free(camera_pos);

    vec3 *temp_pos = vec3_sub(&(box.pos), renderer_camera_position(rndr));
    vec3 *new_dir = vec3_normalize(temp_pos);

    renderer_set_camera_dir(rndr, new_dir);
    free(temp_pos); free(new_dir);

    vec3 up = { 0.0, 1.0, 0.0 };
    renderer_set_camera_up(rndr, &up);
    renderer_set_camera_fov(rndr, 90.0f);
    renderer_set_camera_near(rndr, 0.1f);
    renderer_set_camera_far(rndr, 1000.0);

    glUseProgram(rndr->shader->program);

    rendering_context *rc = make_rendering_context(rndr);

    glClearColor(210.0 / 255.0, 230.0 / 255.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer_start_rendering(rndr, rc);

    vec4 color = { 0.8, 0.4, 0.2, 1.0 };
    rendering_context_set_uniform_vec4(rc, "vecColor", &color);

    vec3 player_shift_min = { 0.5, 0.0, 0.5 };
    vec3 player_shift_max = { 0.5, 2.0, 0.5 };
    vec3 *player_pos_min = vec3_sub(&(box.pos), &player_shift_min);
    vec3 *player_pos_max = vec3_add(&(box.pos), &player_shift_max);

    rendering_context_render_box(rc, player_pos_min, player_pos_max);

    vec4 color2 = { 0.3, 0.9, 0.5, 1.0 };
    vec3 box_temp_pos = { 6.0, 0.0, 4.0 };
    vec3 box_shift_min = { 0.5, 0.0, 0.5 };
    vec3 box_shift_max = { 0.5, 1.0, 0.5 };
    vec3 *box_pos_min = vec3_sub(&box_temp_pos, &box_shift_min);
    vec3 *box_pos_max = vec3_add(&box_temp_pos, &box_shift_max);

    rendering_context_set_uniform_vec4(rc, "vecColor", &color2);
    rendering_context_render_box(rc, box_pos_min, box_pos_max);

    static vec4 color3 = { 0.6, 0.7, 0.9, 1.0 };

    static float verts[] = {
        -30.0, 0.0, -30.0,
        -30.0, 0.0, 30.0,
        30.0, 0.0, 30.0,
        30.0, 0.0, -30.0
    };

    rendering_context_set_uniform_vec4(rc, "vecColor", &color3);
    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, verts, sizeof(verts) / sizeof(float));
    rendering_context_end_render(rc);

    renderer_finish_rendering(rndr, rc);
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisplayMode info;

    SDL_GetCurrentDisplayMode(0, &info);

    application = (app*)malloc(sizeof(app));
    application->w = info.w;
    application->h = info.h;

    printf("w %d, h %d\n", application->w, application->h);

    /* Setup OpenGL 3.2 Context For OS X */
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);

    /* Create a fullscreen window */
    SDL_Window *screen = SDL_CreateWindow("MFGD",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          0, 0,
                                          SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    /* initialize a render context, managed by SDL */
    SDL_GLContext *opengl3_context = SDL_GL_CreateContext(screen);
    SDL_GL_SetSwapInterval(1);

    /* Use GLEW to setup gl Functions */
    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();

    if (glew_status != GLEW_OK) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "GLEW Error",
                                 "Couldn't initialize GLEW",
                                 NULL);
        return -1;
    }

	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    const GLubyte* vendor   = glGetString (GL_VENDOR);
    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version  = glGetString (GL_VERSION);
    const GLubyte* glsl_ver = glGetString (GL_SHADING_LANGUAGE_VERSION);

    printf ( "%s : %s (%s)\n >> GLSL: %s\n",
             vendor,
             renderer,
             version,
             glsl_ver );

    memset(&box, 0, sizeof(character));
    box.gravity.y = -4.0f;

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,

     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,

    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f
    };

    shader *s = make_shader("model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");
    shader_compile(s);

    glUseProgram(s->program);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(s->pos_attr);
    glVertexAttribPointer(s->pos_attr, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

    glEnableVertexAttribArray(s->color_attr);
    glVertexAttribPointer(s->color_attr, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

    memset(&box, 0, sizeof(character));
    box.gravity.y = -4.0;

    int w, h;
    get_screen_size(&w, &h);
    struct s_renderer *r = make_renderer(w, h);
    renderer_initialize(r);

    /* hook renderer up with our shader prog */
    r->shader = s;

    vec3 cam = { 2.5f, 2.5f, 2.0f };
    vec3 c_up = { 0.0f, 0.0f, 1.0f };
    vec3 c_at = { 0.0f, 0.0f, 0.0f };

    mat4x4 *view = look_at(&cam, &c_at, &c_up);

    printf("View Matrix:\n");
    mat4x4_print(view);
    printf("\n");
    fflush(stdout);

    mat4x4 *proj = perspective(45.0f, 800.0f / 600.0f, 1.0, 10.0f);
    printf("Porj Matrix:\n");
    mat4x4_print(proj);
    printf("\n");
    fflush(stdout);

    GLint uniView = glGetUniformLocation(s->program, "view");


    GLint uniProj = glGetUniformLocation(s->program, "proj");

    GLint uniModel = glGetUniformLocation(s->program, "model");

    GLint uniColor = glGetUniformLocation(s->program, "overrideColor");

    int tick = 1000;

    glEnable(GL_DEPTH_TEST);
    uint32_t now = SDL_GetTicks();
    int pause = 0;
    while(true) {
        SDL_Event event;
        int quit = 0;


        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_SPACE)
                    pause = 1;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_SPACE)
                    pause = 0;
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;
                break;
            case SDL_QUIT:
                quit = 1;
                break;
            }
        }

        if (quit == 1) // if received instruction to quit
            break;


        if (!pause)
            now = SDL_GetTicks();

        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 *ident = mat4x4_make_ident(NULL);
        vec3 up = { 0.0f, 0.0f, 1.0f };

        glUniformMatrix4fv(uniView, 1, GL_FALSE, view->m);
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, proj->m);

        float rotate = now / (float)tick * 180.0f;
        rotate = rotate * PI / 180.0f;
        mat4x4 *model = mat4x4_rotate(ident, rotate, &up);
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, model->m);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
        glDepthMask(GL_FALSE);
        glClear(GL_STENCIL_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 36, 6);

        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDepthMask(GL_TRUE);

        mat4x4 *trans = mat4x4_translate(model, 0.0, 0.0, -1.0);
        mat4x4_scale(trans, 1.0, 1.0, -1.0);

        glUniformMatrix4fv(uniModel, 1, GL_FALSE, trans->m);

        glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
        glDisable(GL_STENCIL_TEST);
        SDL_GL_SwapWindow(screen);

        mat4x4_cleanup(trans);
        mat4x4_cleanup(ident);
        mat4x4_cleanup(model);
    }

    shader_cleanup(s);
    glDeleteVertexArrays(1, &vao);

    free(r);

    SDL_GL_DeleteContext(opengl3_context);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
