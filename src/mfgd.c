#include <GL/glew.h>
#include <stdint.h>
#include <SDL.h>

#include "vector.h"

float approach(float goal, float current, float dt) {
    float diff = goal - current;

    if (diff > dt)
        return current + dt;

    if (diff < -dt)
        return current - dt;

    return goal;
}

int main(int argc, char** argv) {
    /* Setup OpenGL 3.2 Context For OS X */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


    /* Create a fullscreen window */
    SDL_Window *screen = SDL_CreateWindow("MFGD",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          0, 0,
                                          SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);


    /* initialize a render context, mark as unusued to surpress warnings */
    SDL_GLContext *opengl3_context = SDL_GL_CreateContext(screen);

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "GLEW Error",
                                 "Couldn't initialize GLEW",
                                 NULL);
        return -1;
    }

    while(true) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);
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

    SDL_GL_DeleteContext(opengl3_context);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
