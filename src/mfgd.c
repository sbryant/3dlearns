#include <stddef.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL.h>

typedef struct s_vector {
    float x;
    float y;
} vector;

typedef struct s_point {
    float x;
    float y;
} point;


point* point_add_vector(point* const p, vector* const v) {
    point *p2 = (point*)calloc(1, sizeof(point));

    p2->x = p->x + v->x;
    p2->y = p->y + v->y;

    return p2;
}

vector* point_sub(point* const a, point* const b) {
    vector* v = (vector*)calloc(1, sizeof(vector));
    v->x = a->x - b->x;
    v->y = a->y - b->y;

    return v;
}


float vector_length(vector* const v) {
    return sqrtf((v->x * v->x) + (v->y * v->y));
}

float vector_length_square(vector* const v) {
    return (v->x * v->x) + (v->y * v->y);
}

vector* vector_add(vector* const v1, vector* const v2) {
    vector *vr = (vector*)calloc(1, sizeof(vector));

    vr->x = v1->x + v2->x;
    vr->y = v1->y + v2->y;

    return vr;
}

vector* vector_sub(vector* const v1, vector* const v2) {
    vector *vr = (vector*)calloc(1, sizeof(vector));

    vr->x = v1->x - v2->x;
    vr->y = v1->y - v2->y;

    return vr;
}

/* Vector scaling functions */

vector *vector_mul_scalar(vector* const v, float const s) {
    vector* v2 = (vector *)calloc(1, sizeof(vector));

    v2->x = v->x * s;
    v2->y = v->y * s ;

    return v2;
}

vector *vector_div_scalar(vector* const v, float const s) {
    vector* v2 = (vector *)calloc(1, sizeof(vector));

    v2->x = v->x / s;
    v2->y = v->y / s ;

    return v2;
}


vector *vector_normalize(vector* const v) {
    vector *v2  = (vector *)calloc(1, sizeof(vector));
    float v_len = vector_length(v);

    v2->x = v->x / v_len;
    v2->y = v->y / v_len;

    return v2;
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);

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
