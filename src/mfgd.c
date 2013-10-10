#include <GL/glew.h>
#include <stdint.h>
#include <SDL.h>

#include "matrix.h"
#include "vector.h"
#include "utils.h"
#include "assert.h"

float approach(float goal, float current, float dt) {
    float diff = goal - current;

    if (diff > dt)
        return current + dt;

    if (diff < -dt)
        return current - dt;

    return goal;
}

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

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);

    /* Setup OpenGL 3.2 Context For OS X */
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    /* Create a fullscreen window */
    SDL_Window *screen = SDL_CreateWindow("MFGD",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
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

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    char* vert_source = read_shader("shaders/simple_vert.glsl");
    assert(vert_source != NULL);

    glShaderSource(vert_shader, 1, (const GLchar**)&vert_source, NULL);

    glCompileShader(vert_shader);

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* frag_source = (GLchar*)read_shader("shaders/simple_frag.glsl");
    assert(frag_source != NULL);

    glShaderSource(frag_shader, 1, (const GLchar**)&frag_source, NULL);
    glCompileShader(frag_shader);

    GLuint shader_prog = glCreateProgram();
    glAttachShader(shader_prog, vert_shader);
    glAttachShader(shader_prog, frag_shader);

    glUseProgram( shader_prog );

    glBindFragDataLocation( shader_prog, 0, "outColor" );

    glLinkProgram( shader_prog );

    GLint pos_attr = glGetAttribLocation( shader_prog, "position" );
    glEnableVertexAttribArray(pos_attr);
    glVertexAttribPointer( pos_attr, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0 );

    GLint color_attr = glGetAttribLocation( shader_prog, "color" );
    glEnableVertexAttribArray(color_attr);
    glVertexAttribPointer( color_attr, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(float)));

    GLuint model_uni = glGetUniformLocation(shader_prog, "model");
    GLuint view_uni = glGetUniformLocation(shader_prog, "view");
    GLuint proj_uni = glGetUniformLocation(shader_prog, "proj");

    vec3 camera = { 0.000001, 0.00001, 8.0 };
    vec3 center = { 0.0, 0.0, 0.0 };
    vec3 up = { 0.0, 0.0, 1.0 };
    vec3 axis = { 0.0, 0.0, 1.0 };

    mat4x4* view = look_at(&camera, &center, &up);
    printf("View Matrix:\n");
    mat4x4_print(view);
    printf("\n");
    float* view_data = mat4x4_make_array(view);
    mat4x4_cleanup(view);

    mat4x4 *ident = mat4x4_make_ident(NULL);
    float *ident_data = mat4x4_make_array(ident);

    float fovy_rad = 45.0 * PI / 180.0;
    mat4x4 *proj =  perspective(fovy_rad, 800.0 / 600.0 , 1.0, 10.0f);
    printf("PerspectiveProj Matrix:\n");
    mat4x4_print(proj);
    printf("\n");
    float* proj_data = mat4x4_make_array(proj);
    mat4x4_cleanup(proj);

    uint64_t old = SDL_GetPerformanceCounter();
     unsigned long tick = SDL_GetPerformanceFrequency();
    int pause = 0;

    uint64_t now = SDL_GetPerformanceCounter();
    float factor = (float)now / tick;
    float a =  20.0 * (PI/180.0);

    glEnable(GL_DEPTH_TEST);

    /* get handle to hold verts we upload */
    while(true) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        glUseProgram( shader_prog );
        //  float dt = (float)(now - old) / tick;

        if (!pause)
            now = SDL_GetPerformanceCounter();
        factor = (float)now / tick;
        old = now;


        mat4x4 *trans = mat4x4_rotate(ident, factor * a, &axis);
        float *data =  mat4x4_make_array(trans);

        //view_data = ident_data;
        //proj_data = ident_data;
        glUniformMatrix4fv(view_uni, 1, GL_FALSE, view_data);
        glUniformMatrix4fv(proj_uni, 1, GL_FALSE, proj_data);
        glUniformMatrix4fv(model_uni, 1, GL_FALSE, data);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        SDL_GL_SwapWindow(screen);

        SDL_Event event;
        int quit = 0;
        mat4x4_cleanup(trans);
        free(data);

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

    mat4x4_cleanup(ident);
    free(ident_data);

    glDeleteProgram(shader_prog);
    glDeleteShader(frag_shader);
    glDeleteShader(vert_shader);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(opengl3_context);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
