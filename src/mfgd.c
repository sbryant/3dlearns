#if defined(_WIN32) 
#include <Windows.h>
#endif

#include <glew.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#undef main

#include "linmath.h"
#include "utils.h"
#include "shader.h"


struct s_RenderInfo {
	shader shaderInfo;
	unsigned int width, height;
};

static struct s_RenderInfo renderInfo = { 0 };

void update(float dt) {
}

void my_draw() {
	glUseProgram(renderInfo.shaderInfo.program);

	glClearColor(210.f / 255.f, 230.f / 255.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

#if defined(_WIN32) && 0
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR     lpCmdLine,
	int       nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
	SDL_Init(SDL_INIT_VIDEO);
	SDL_DisplayMode info;

	SDL_GetCurrentDisplayMode(0, &info);

	renderInfo.width = (unsigned int)info.w;
	renderInfo.height = (unsigned int)info.h;

	printf("w %d, h %d\n", info.w, info.h);

	/* Setup OpenGL 3.2 Context For OS X */
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
#if defined(__APPLE_CC__)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
#endif
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	/* Create a fullscreen window */
	SDL_Window *screen = SDL_CreateWindow("MFGD",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_OPENGL);

	SDL_SetRelativeMouseMode(true);

	/* initialize a render context, managed by SDL */
	SDL_GLContext *opengl_context = SDL_GL_CreateContext(screen);
	SDL_GL_SetSwapInterval(1);

	GLenum err = glGetError();
	if (err != 0) {
		printf("GLError: %d\n", err);
	}

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
	err = glGetError();
	err = err == GL_INVALID_ENUM ? 0 : err;
	if (err != 0) {
		printf("GLError: %d\n", err);
	}

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glsl_ver = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("%s : %s (%s)\n >> GLSL: %s\n",
		vendor,
		renderer,
		version,
		glsl_ver);

	init_shader(&renderInfo.shaderInfo, "model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");
	shader_compile(&renderInfo.shaderInfo);

	glGenVertexArrays(1, &renderInfo.shaderInfo.vao);
	glBindVertexArray(renderInfo.shaderInfo.vao);

	uint32_t old = SDL_GetTicks();
	uint32_t now = SDL_GetTicks();
	int pause = 0;
	while (true) {
		SDL_Event event;
		int quit = 0;
		int rel_x, rel_y;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				break;
			case SDL_KEYUP:
				break;
			case SDL_MOUSEMOTION:
				SDL_GetRelativeMouseState(&rel_x, &rel_y);
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			}
		}

		if (quit == 1) // if received instruction to quit
			break;

		if (!pause) {
			old = now;
			now = SDL_GetTicks();
		}

		float dt = (float)(now - old);

		if (dt > 16.0f) {
			dt = 16.0f;
		}

		update(dt);
		my_draw();
		SDL_GL_SwapWindow(screen);

	}

	glDeleteVertexArrays(1, &renderInfo.shaderInfo.vao);
	shader_cleanup(&renderInfo.shaderInfo);

	SDL_GL_DeleteContext(opengl_context);
	SDL_DestroyWindow(screen);
	SDL_Quit();

	return 0;
}
