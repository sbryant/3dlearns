#if defined(_WIN32) 
#include <Windows.h>
#endif

#define STB_DEFINE 
#define STBI_PNG_ONLY
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "stb.h"
#include "stb_image.h"
#include "stb_truetype.h"

#include <glew.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#undef main

#include "linmath.h"
#include "utils.h"
#include "shader.h"
#include "sb_debug.h"

#define BITMAP_BYTES_PER_PIXEL 1

struct sb_bitmap {
	void* data;
	int width, height;
	int pitch;
};

struct sb_bitmap* make_empty_bitmap(int width, int height, int zero) {
	assert(height != 0.0f && "Height needs to be greater than zero");

	struct sb_bitmap* bitmap;

	if (zero == 1)
		bitmap = calloc(1, sizeof(*bitmap));
	else
		bitmap = calloc(1, sizeof(*bitmap));

	/* good values */
	bitmap->width = width;
	bitmap->height = height;
	bitmap->pitch = BITMAP_BYTES_PER_PIXEL;

	int bitmap_size = height * width * BITMAP_BYTES_PER_PIXEL;

	if (zero == 1)
		bitmap->data = (void*)calloc(1, bitmap_size);
	else 
		bitmap->data = (void*)malloc(bitmap_size);

	return bitmap;
}

static stbtt_bakedchar cdata[128];
#define BITMAP_WIDTH 2048
#define BITMAP_HEIGHT 2048

static struct sb_bitmap* sb_bitmap_font(width, height) {
	const char* font_file_path = "C:/Windows/Fonts/cour.ttf";
	struct sb_debug_file_read_result* font_file = debug_read_entire_file(font_file_path);
	struct sb_bitmap* bitmap = make_empty_bitmap(width, height, 1);

	stbtt_fontinfo font;
	uint8_t* data = (uint8_t*)(font_file->data);

	/* gets all characters from ! - ~ (includes a-Z 0-9) */
	stbtt_BakeFontBitmap(data, 0, 256.0, bitmap->data, bitmap->width, bitmap->height, '!', ('~' - '!')+1, &cdata[0]);

	return bitmap;
}

static void sb_bitmap_free(struct sb_bitmap* font) {
	assert(font != NULL && font->data != NULL && "Font and Font data need to be valid pointers");
	free(font->data);
	free(font);
}

struct sb_render_group {
	shader shader_info;
	int vao;
	int vbo;
	int texture;
	mat4x4 projection;
};

static struct sb_render_group render_group = { 0 };

void update(float dt) {
}

void my_draw() {
	glClearColor(210.f / 255.f, 230.f / 255.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_use(&render_group.shader_info);
	glBindVertexArray(render_group.vao);

	int location = glGetUniformLocation(render_group.shader_info.program, "projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)render_group.projection);

	glDrawArrays(GL_TRIANGLES, 0, 6);
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
	fprintf(stderr, "w %d, h %d\n", info.w, info.h);

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
	SDL_Window *screen = SDL_CreateWindow("OpenGL",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_OPENGL);

	SDL_SetRelativeMouseMode(1);

	/* initialize a render context, managed by SDL */
	SDL_GLContext *opengl_context = SDL_GL_CreateContext(screen);
	SDL_GL_SetSwapInterval(1);

	GLenum err = glGetError();
	if (err != 0) {
		fprintf(stderr, "GLError: %d\n", err);
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

	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	err = glGetError();
	err = err == GL_INVALID_ENUM ? 0 : err;
	if (err != 0) {
		printf("GLError: %d\n", err);
	}

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glsl_ver = glGetString(GL_SHADING_LANGUAGE_VERSION);

	fprintf(stderr, "%s : %s (%s)\n >> GLSL: %s\n",
		vendor,
		renderer,
		version,
		glsl_ver);

	shader_compile(&render_group.shader_info, "model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");

	mat4x4_identity(&render_group.projection);

	float aspect = (float)info.w / (float)info.h;

	/* normal ortho projection from -1,1 is TL and 1.0,-1.0 is BR */
	mat4x4_ortho(&render_group.projection, -aspect, aspect, -1.0f, 1.0f, 1.0f, -1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glGenVertexArrays(1, &render_group.vao);
	glBindVertexArray(render_group.vao);
	int tex; glGenTextures(1, &tex);

	/* generate bitmap font texture */
	struct sb_bitmap* font = sb_bitmap_font(2048, 2048);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->width, font->height, 0, GL_RED, GL_UNSIGNED_BYTE, font->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	int vbo; glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	const float vertices[] =
	{
	/*   x     y     z	 s  t   */
		-0.5,  0.5,  0,  0, 0,
		 0.5,  0.5,  0,  1, 0,
		 0.5, -0.5,  0,  1, 1,
		 0.5, -0.5,  0,  1, 1,
		-0.5, -0.5,  0,  0, 1,
		-0.5,  0.5,  0,  0, 0
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	int pos_attr = glGetAttribLocation(render_group.shader_info.program, "in_position");
	int uv_attr = glGetAttribLocation(render_group.shader_info.program, "in_tex");

	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(pos_attr);

	glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(uv_attr);

	uint32_t old = SDL_GetTicks();
	uint32_t now = SDL_GetTicks();
	int pause = 0;
	while (1) {
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

	sb_bitmap_free(font);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &render_group.vao);
	glDeleteVertexArrays(1, &render_group.vao);
	shader_cleanup(&render_group.shader_info);

	SDL_GL_DeleteContext(opengl_context);
	SDL_DestroyWindow(screen);
	SDL_Quit();

	return 0;
}
