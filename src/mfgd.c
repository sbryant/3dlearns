#include <GL/glew.h>
#include <stdint.h>
#include <SDL.h>

#include "matrix.h"
#include "vector.h"
#include "utils.h"
#include "shader.h"





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
                                          800, 600,
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

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    float verts[] = {
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,

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
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo );
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    shader *s = make_shader("model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");
    shader_compile(s);

    glUseProgram(s->program);

    glEnableVertexAttribArray(s->pos_attr);
    glVertexAttribPointer( s->pos_attr, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0 );

    glEnableVertexAttribArray(s->color_attr);
    glVertexAttribPointer( s->color_attr, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(float)));


    GLuint model_uni = glGetUniformLocation(s->program, "model");
    GLuint view_uni = glGetUniformLocation(s->program, "view");
    GLuint proj_uni = glGetUniformLocation(s->program, "proj");

    vec3 camera = { 0.0, 0.0, 8.0 };
    vec3 center = { 0.0, 0.0, 0.0};
    vec3 up = { 0.0, 1.0, 0.0 };
    vec3 axis = { 0.0, 1.0, 0.0 };

    mat4x4* view = look_at(&camera, &center, &up);
    printf("View Matrix:\n");
    mat4x4_print(view);
    printf("\n");

    float fovy_rad = 45.0 * PI / 180.0;
    mat4x4 *proj =  perspective(fovy_rad, 16.0 / 9.0 , 0.1, 1000.0f);
    printf("PerspectiveProj Matrix:\n");
    mat4x4_print(proj);
    printf("\n");

    uint64_t old = SDL_GetPerformanceCounter();
    unsigned long tick = SDL_GetPerformanceFrequency();
    int pause = 0;

    uint64_t now = SDL_GetPerformanceCounter();
    float factor = 0.0;
    float a =  180.0 * (PI/180.0);
    mat4x4 *ident = mat4x4_make_ident(NULL);

    /* get handle to hold verts we upload */
    while(true) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        glUseProgram( s->program );
        //  float dt = (float)(now - old) / tick;

        if (!pause) {
            old = now;
            now = SDL_GetPerformanceCounter();
        }

        factor = now / (float)tick;

        mat4x4 *trans = mat4x4_rotate(ident, factor * a, &axis);
        glUniformMatrix4fv(view_uni, 1, GL_FALSE, view->m);
        glUniformMatrix4fv(proj_uni, 1, GL_FALSE, proj->m);
        glUniformMatrix4fv(model_uni, 1, GL_FALSE, trans->m);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        SDL_GL_SwapWindow(screen);

        SDL_Event event;
        int quit = 0;
        mat4x4_cleanup(trans);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_SPACE)
                    pause = 1;
                break;
            case SDL_KEYUP:
                // if escape is pressed, quit
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
    }

    mat4x4_cleanup(view);
    mat4x4_cleanup(proj);
    mat4x4_cleanup(ident);

    shader_cleanup(s);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    free(r);

    SDL_GL_DeleteContext(opengl3_context);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
