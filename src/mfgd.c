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

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);

    /* Setup OpenGL 3.2 Context For OS X */
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /* Create a fullscreen window */
    SDL_Window *screen = SDL_CreateWindow("MFGD",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          0, 0,
                                          SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    /* initialize a render context, managed by SDL */
    SDL_GLContext *opengl3_context = SDL_GL_CreateContext(screen);

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
        -0.5f,  0.5f, 1.0, 0.0, 0.0,
         0.5f,  0.5f, 0.0, 1.0, 0.0,
         0.5f, -0.5f, 0.0, 0.0, 1.0,
        -0.5f, -0.5f, 1.0, 1.0, 0.0
    };

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

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
    glVertexAttribPointer( pos_attr, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 );

    GLint color_attr = glGetAttribLocation( shader_prog, "color" );
    glEnableVertexAttribArray(color_attr);
    glVertexAttribPointer( color_attr, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2 * sizeof(float)));

    /* get handle to hold verts we upload */
    while(true) {
		glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        glUseProgram( shader_prog );

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(screen);

        SDL_Event event;
        int quit = 0;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
                // if escape is pressed, quit
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
