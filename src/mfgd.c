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

struct sb_debug_file_read_result {
	long size;
	void* data;
};

static struct sb_debug_file_read_result* debug_read_entire_file(const char* path) {
	struct sb_debug_file_read_result* read_result = calloc(1, sizeof(*read_result));

	HANDLE file_handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file_handle == INVALID_HANDLE_VALUE) {
		long error = GetLastError();
		return NULL;
	}

	LARGE_INTEGER file_size = { 0 };
	GetFileSizeEx(file_handle, &file_size);
	read_result->size = file_size.QuadPart;

	fprintf(stderr, "%s has a file size of: %d bytes\n", path, read_result->size);

	read_result->data = malloc(read_result->size);
	long bytes_read = 0;
	ReadFile(file_handle, read_result->data, read_result->size, &bytes_read, NULL);
	fprintf(stderr, "read %d bytes / %d bytes\n", bytes_read, read_result->size);
	fflush(stderr);

	CloseHandle(file_handle);

	return read_result;
}

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

static stbtt_bakedchar cdata[96];

static struct sb_bitmap* sb_bitmap_font() {
	const char* font_file_path = "C:/Windows/Fonts/arial.ttf";
	struct sb_debug_file_read_result* font_file = debug_read_entire_file(font_file_path);

	stbtt_fontinfo font;
	uint8_t* data = (uint8_t*)(font_file->data);
	stbtt_InitFont(&font, data, stbtt_GetFontOffsetForIndex(data, 0));

	int ascent, descent, line_gap, x = 0;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);

	float scale = stbtt_ScaleForPixelHeight(&font, 60);
	ascent *= scale;
	struct sb_bitmap* bitmap = make_empty_bitmap(1024, 1024, 1);

	const char* baked_string = "stb_truetype demo";
	char* ptr = baked_string;

	while (*ptr) {
		char c = *ptr;
		int x1, y1, x2, y2;
		stbtt_GetCodepointBitmapBox(&font, c, scale, scale, &x1, &y1, &x2, &y2);

		float y = ascent + y1;

		int byte_offset = x + y * 1024;

		stbtt_MakeCodepointBitmap(&font, (uint8_t*)bitmap->data + byte_offset, x2 - x1, y2 - y1, 1024, scale, scale, c);

		int ax;
		stbtt_GetCodepointHMetrics(&font, c, &ax, NULL);
		x += ax * scale;

		ptr++;

		/* advance kerning n-1 times */
		if (*ptr) {
			int kern = stbtt_GetCodepointKernAdvance(&font, c, *ptr);
			x += kern * scale;
		}
	}
	return bitmap;
}

static void sb_bitmap_free(struct sb_bitmap* font) {
	assert(font != NULL && font->data != NULL && "Font and Font data need to be valid pointers");
	free(font->data);
	free(font);
}

struct s_RenderInfo {
	shader shaderInfo;
	unsigned int width, height;
	int vao;
};

static struct s_RenderInfo renderInfo = { 0 };

void update(float dt) {
}

void my_draw() {
	glClearColor(210.f / 255.f, 230.f / 255.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_use(&renderInfo.shaderInfo);
	glBindVertexArray(renderInfo.vao);
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
	SDL_Window *screen = SDL_CreateWindow("OpenGL",
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

	shader_compile(&renderInfo.shaderInfo, "model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");

	glGenVertexArrays(1, &renderInfo.vao);
	glBindVertexArray(renderInfo.vao);
	int tex; glGenTextures(1, &tex);

	/* generate bitmap font texture */
	struct sb_bitmap* font = sb_bitmap_font();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font->width, font->height, 0, GL_RED, GL_UNSIGNED_BYTE, font->data);

	int vbo; glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	const float vertices[] =
	{
	/*	 x     y	 z	 s  t  */
		-0.5,  0.5,  0,  0, 0,
		 0.5,  0.5,  0,  1, 0,
		 0.5, -0.5,  0,  1, 1,
		 0.5, -0.5,  0,  1, 1,
		-0.5, -0.5,  0,  0, 1,
		-0.5,  0.5,  0,  0, 0
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	int pos_attr = glGetAttribLocation(renderInfo.shaderInfo.program, "in_position");
	int uv_attr = glGetAttribLocation(renderInfo.shaderInfo.program, "in_tex");

	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(pos_attr);

	glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(uv_attr);

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

	sb_bitmap_free(font);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &renderInfo.vao);
	glDeleteVertexArrays(1, &renderInfo.vao);
	shader_cleanup(&renderInfo.shaderInfo);

	SDL_GL_DeleteContext(opengl_context);
	SDL_DestroyWindow(screen);
	SDL_Quit();

	return 0;
}
