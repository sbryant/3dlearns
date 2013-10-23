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

    vec3 *temp_vel = vec3_mul_scalar(&(box.velocity), dt);
    vec3 *new_pos = vec3_add(&(box.pos), temp_vel);

    vec3_set_vec3(&(box.pos), new_pos);
    free(temp_vel); free(new_pos);

    vec3 *temp_grav = vec3_mul_scalar(&(box.gravity), dt);
    vec3 *new_vel = vec3_add(&(box.velocity), temp_grav);

    vec3_set_vec3(&(box.velocity), new_vel);
    free(new_vel); free(temp_grav);

    if (box.pos.y < 0.0f) {
        box.pos.y = 0.0f;
        box.velocity.y = 0.0f;
    }

}

void draw(renderer *rndr) {
    vec3 shift = { 0.0, 4.0, -6.0 };
    vec3 *camera_pos = vec3_add(&(box.pos), &shift);
    renderer_set_camera_position(rndr, camera_pos);

    vec3 *temp_pos = vec3_sub(&(box.pos), renderer_camera_position(rndr));
    vec3 *new_dir = vec3_normalize(temp_pos);

    renderer_set_camera_dir(rndr, new_dir);
    free(temp_pos);

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

    vec3 player_shift_min = { 0.5f, 0.0f, 0.5f };
    vec3 player_shift_max = { 0.5f, 2.0f, 0.5f };
    vec3 *pos = &box.pos;
    vec3 *player_pos_min = vec3_sub(pos, &player_shift_min);
    vec3 *player_pos_max = vec3_add(pos, &player_shift_max);

    rendering_context_render_box(rc, player_pos_min, player_pos_max);

    vec4 color2 = { 0.3, 0.9, 0.5, 1.0 };
    vec3 box_temp_pos = { 6.0f, 0.0f, 4.0f };
    vec3 box_shift_min = { 0.5f, 0.0f, 0.5f };
    vec3 box_shift_max = { 0.5f, 1.0f, 0.5f };
    vec3 *box_pos_min = vec3_sub(&box_temp_pos, &box_shift_min);
    vec3 *box_pos_max = vec3_add(&box_temp_pos, &box_shift_max);

    rendering_context_set_uniform_vec4(rc, "vecColor", &color2);
    rendering_context_render_box(rc, box_pos_min, box_pos_max);

    static vec4 color3 = { 0.6, 0.7, 0.9, 1.0 };

    static float verts[] = {
        -30.0, 0.0, -30.0, 1.0, 1.0, 1.0,
        -30.0, 0.0,  30.0, 1.0, 1.0, 1.0,
        30.0, 0.0,  30.0, 1.0, 1.0, 1.0,
        30.0, 0.0, -30.0, 1.0, 1.0, 1.0
    };

    rendering_context_set_uniform_vec4(rc, "vecColor", &color3);
    rendering_context_begin_render_tri_fan(rc);
    rendering_context_render_tri_face(rc, verts, sizeof(verts) / sizeof(float) / 6.0);
    rendering_context_end_render(rc);

    renderer_finish_rendering(rndr, rc);

    free(box_pos_max);
    free(box_pos_min);

    free(player_pos_max);
    free(player_pos_min);
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
                                          1680, 1050,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

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
    box.pos.x = box.pos.y = box.pos.z = 0.0f;
    box.velocity.x = box.velocity.y = box.velocity.z = 0.0f;
    box.velocity_goal.x = box.velocity_goal.y = box.velocity_goal.z = 0.0f;
    box.gravity.x = box.gravity.y = box.gravity.z = 0.0f;
    box.gravity.y = -4.0f;

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    shader *s = make_shader("model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");
    shader_compile(s);

    int w, h;
    get_screen_size(&w, &h);
    struct s_renderer *r = make_renderer(w, h);
    renderer_initialize(r);

    /* hook renderer up with our shader prog */
    r->shader = s;

    glEnable(GL_DEPTH_TEST);
    uint32_t old = SDL_GetTicks();
    uint32_t now = SDL_GetTicks();
    int pause = 0;
    while(true) {
        SDL_Event event;
        int quit = 0;
        int w_down, a_down, s_down, d_down = 0;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_p)
                    pause = 1;
                if (event.key.keysym.sym == SDLK_w)
                    w_down = 1;
                if (event.key.keysym.sym == SDLK_a)
                    a_down = 1;
                if (event.key.keysym.sym == SDLK_s)
                    s_down = 1;
                if (event.key.keysym.sym == SDLK_d)
                    d_down = 1;
                if (event.key.keysym.sym == SDLK_SPACE)
                    box.velocity.y = 2.0f;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_p)
                    pause = 0;
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;
                if (event.key.keysym.sym == SDLK_w)
                    w_down = 0; box.velocity_goal.z = 0.0f;
                if (event.key.keysym.sym == SDLK_a)
                    a_down = 0; box.velocity_goal.x = 0.0f;
                if (event.key.keysym.sym == SDLK_s)
                    s_down = 0; box.velocity_goal.z = 0.0f;
                if (event.key.keysym.sym == SDLK_d)
                    d_down = 0; box.velocity_goal.x = 0.0;
                if (event.key.keysym.sym == SDLK_r)
                    box.pos.x = box.pos.y = box.pos.z = 0.0f;

                break;
            case SDL_QUIT:
                quit = 1;
                break;
            }
        }

        if (w_down)
            box.velocity_goal.z = 15.0f;
        if (a_down)
            box.velocity_goal.x = 15.0f;
        if (s_down)
            box.velocity_goal.z = -15.0f;
        if (d_down)
            box.velocity_goal.x = -15.0f;

        if (quit == 1) // if received instruction to quit
            break;

        if (!pause) {
            old = now;
            now = SDL_GetTicks();
        }

        float dt = now - old;

        if (dt > 0.15f)
            dt = 0.15f;

        update(dt);
        draw(r);
        SDL_GL_SwapWindow(screen);

    }

    shader_cleanup(s);
    glDeleteVertexArrays(1, &vao);

    free(r);

    SDL_GL_DeleteContext(opengl3_context);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
