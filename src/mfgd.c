/*  
 *	TODO:
 *	- Draw colored (vertex) quads tiled across the screen
 *	- include some spacing between tiles
 *	
 *	VISITED (not sure if done):
 *	- Bitmap based font rendering
 *	- easy way to accumulate text that's spit out at the end of a frame 
 */

#if defined(_WIN32) 
#include <Windows.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)
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


#define BITMAP_WIDTH 512
#define BITMAP_HEIGHT 512
#define BITMAP_BYTES_PER_PIXEL 1
#define FONT_SCALE 16
#define HALF_FONT_SCALE (FONT_SCALE / 2.0)

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

static struct sb_bitmap* sb_bitmap_font(width, height) {
	const char* font_file_path = "C:/Windows/Fonts/cour.ttf";
	struct sb_debug_file_read_result* font_file = debug_read_entire_file(font_file_path);
	struct sb_bitmap* bitmap = make_empty_bitmap(width, height, 1);
	uint8_t* data = (uint8_t*)(font_file->data);

	/* gets all characters from SPC - ~ (includes a-Z 0-9) */
	stbtt_BakeFontBitmap(data, 0, FONT_SCALE, bitmap->data, bitmap->width, bitmap->height, ' ', ('~' - ' ')+1, &cdata[0]);

	return bitmap;
}

static void sb_bitmap_free(struct sb_bitmap* font) {
	assert(font != NULL && font->data != NULL && "Font and Font data need to be valid pointers");
	free(font->data);
	free(font);
}

struct sb_bitmap* make_bitmap_font(int* texture, int width, int height) {
	assert(texture != NULL && width > 0 && height > 0);
	glGenTextures(1, texture);

	/* generate bitmap font texture */
	struct sb_bitmap* font = sb_bitmap_font(width, height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);

	/* default assumes 4byte alignment */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->width, font->height, 0, GL_RED, GL_UNSIGNED_BYTE, font->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return font;
}

struct sb_render_group {
	shader shader_info;
	int vao;
	int vbo;
	int texture;
	mat4x4 projection;
};

/* debug performance counters as seen from handemade hero */
enum sb_debug_cycle_counter_group {
	SB_DEBUG_CYCLE_COUNTER__UPDATE_AND_RENDER,
	SB_DEBUG_CYCLE_COUNTER__PROGRAM_SETUP,
	SB_DEBUG_CYCLE_COUNTER__RENDER_GROUP_RENDER,

	SB_DEBUG_CYCLE_COUNTER_COUNT
};

struct sb_debug_cycle_counter {
	uint64_t cycle_count;
	uint32_t hit_count;
};

static struct sb_debug_cycle_counter sb_debug_cycle_counters[SB_DEBUG_CYCLE_COUNTER_COUNT] = { 0 };

/* use the enum sb_debug_cycle_counter_group for IDs e.g UPDATE_AND_RENDER */
#define sb_debug_cycle_begin_timed_block(ID) uint64_t __start_cycle_count__##ID = __rdtsc();
#define sb_debug_cycle_end_timed_block(ID) sb_debug_cycle_counters[SB_DEBUG_CYCLE_COUNTER__##ID].cycle_count += __rdtsc() - __start_cycle_count__##ID; sb_debug_cycle_counters[SB_DEBUG_CYCLE_COUNTER__##ID].hit_count++;
#define sb_debug_cycle_end_counted(ID, count) sb_debug_cycle_counters[SB_DEBUG_CYCLE_COUNTER__##ID].cycle_count += __rdtsc() - __start_cycle_count__##ID; sb_debug_cycle_counters[SB_DEBUG_CYCLE_COUNTER__##ID].hit_count += count;

/* potentially 1k characters */
static float debug_text_vert_buffer[1024 * 30];

static float atY = HALF_FONT_SCALE + 2;
static float atX = 2;
static int num_text_lines = 0;
static int num_of_chars = 0;

static void sb_debug_render_text(const char* string, struct sb_bitmap* font) {
	/* 3 verts and two tex coords per tri and 2 tris per quad = (x, y, z, s, t) * 3 * 2 */
	int string_verts_count = 30 * strlen(string);

	if (string_verts_count > (sizeof(debug_text_vert_buffer) / sizeof(debug_text_vert_buffer[0]))) {
		string_verts_count = (sizeof(debug_text_vert_buffer) / sizeof(debug_text_vert_buffer[0]));
	}

	/*
	vert format is x, y, z, s, t - starting at top left and renders clockwise
	*/
	int i = num_of_chars * 30; // there are 30 floats per quad
	for (char* c = (char*)string; *c; ++c) {
		/* only supports the ASCII printable character code points */
		if (*c >= 32 && *c < 128) {
			stbtt_aligned_quad quad;
			stbtt_GetBakedQuad(cdata, font->width, font->height, *c - 32, &atX, &atY, &quad, 1);

			/* top left */
			debug_text_vert_buffer[i + 0] = quad.x0;
			debug_text_vert_buffer[i + 1] = quad.y0;
			debug_text_vert_buffer[i + 2] = 0.0f;
			debug_text_vert_buffer[i + 3] = quad.s0;
			debug_text_vert_buffer[i + 4] = quad.t0;

			/* top right */
			debug_text_vert_buffer[i + 5] = quad.x1;
			debug_text_vert_buffer[i + 6] = quad.y0;
			debug_text_vert_buffer[i + 7] = 0.0f;
			debug_text_vert_buffer[i + 8] = quad.s1;
			debug_text_vert_buffer[i + 9] = quad.t0;

			/* bottom right */
			debug_text_vert_buffer[i + 10] = quad.x1;
			debug_text_vert_buffer[i + 11] = quad.y1;
			debug_text_vert_buffer[i + 12] = 0.0f;
			debug_text_vert_buffer[i + 13] = quad.s1;
			debug_text_vert_buffer[i + 14] = quad.t1;

			/* bottom right */
			debug_text_vert_buffer[i + 15] = quad.x1;
			debug_text_vert_buffer[i + 16] = quad.y1;
			debug_text_vert_buffer[i + 17] = 0.0f;
			debug_text_vert_buffer[i + 18] = quad.s1;
			debug_text_vert_buffer[i + 19] = quad.t1;

			/* bottom left */
			debug_text_vert_buffer[i + 20] = quad.x0;
			debug_text_vert_buffer[i + 21] = quad.y1;
			debug_text_vert_buffer[i + 22] = 0.0f;
			debug_text_vert_buffer[i + 23] = quad.s0;
			debug_text_vert_buffer[i + 24] = quad.t1;

			/* top left */
			debug_text_vert_buffer[i + 25] = quad.x0;
			debug_text_vert_buffer[i + 26] = quad.y0;
			debug_text_vert_buffer[i + 27] = 0.0f;
			debug_text_vert_buffer[i + 28] = quad.s0;
			debug_text_vert_buffer[i + 29] = quad.t0;

			i += 30;
			++num_of_chars;
		}
	}

	/* move down a single line of text + 5 pixels */
	atY += (num_text_lines + FONT_SCALE);
	atX = 2;
	++num_text_lines;
}

static void sb_debug_overlay_cycle_counters(struct sb_bitmap* font) {
	const char* counter_labels[SB_DEBUG_CYCLE_COUNTER_COUNT] = {
		"Update and Render",
		"Program Start",
		"Rendergroup Render"
	};

	sb_debug_render_text("DEBUG CYCLE COUNTERS", font);
	//fprintf(stderr, "DEBUG CYCLE COUNTERS:\n");
	char string_buff[1024];
	for (int i = 0; i < (sizeof(sb_debug_cycle_counters) / sizeof(sb_debug_cycle_counters[0])); ++i) {
		struct sb_debug_cycle_counter* counter = sb_debug_cycle_counters + i;
		if (counter->hit_count) {

			snprintf(string_buff, sizeof(string_buff), 
				"%s: %I64dcy %uh %I64dcy/h", 
				counter_labels[i],
				counter->cycle_count,	
				counter->hit_count, 
				counter->cycle_count / counter->hit_count);

			sb_debug_render_text(string_buff, font);

			counter->cycle_count = 0;
			counter->hit_count = 0;
		}
	}
}
static struct sb_render_group render_group = { 0 };
static struct sb_render_group debug_render_group = { 0 };

void update(float dt) {
}

static void my_draw() {
	glClearColor(210.f / 255.f, 230.f / 255.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_use(&render_group.shader_info);
	glBindBuffer(GL_ARRAY_BUFFER, render_group.vbo);
	glBindTexture(GL_TEXTURE_2D, render_group.texture);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
	int location = glGetUniformLocation(render_group.shader_info.program, "projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)render_group.projection);

	location = glGetUniformLocation(render_group.shader_info.program, "view");
	vec3 eye = { 0.0f, 0.0, 3.0f };
	vec3 point = { 0.0f, 0.0f, 0.0f };
	vec3 up = { 0.0, 1.0f, 0.0f };
	mat4x4 view;  mat4x4_identity(view);
	mat4x4_look_at(view, eye, point, up);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)view);

	location = glGetUniformLocation(render_group.shader_info.program, "model");
	mat4x4 model;
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			mat4x4_identity(model);
			mat4x4_translate_in_place(model, -1 + x, 1 - y, 0);
			glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}

GLsync fence = 0;
static void debug_draw() {
	mat4x4 ident; mat4x4_identity(ident);
	glUseProgram(debug_render_group.shader_info.program);
	glBindBuffer(GL_ARRAY_BUFFER, debug_render_group.vbo);
	glBindTexture(GL_TEXTURE_2D, debug_render_group.texture);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	int text_quad_buff_size = 30 * num_of_chars * sizeof(float);

	void* old_data = glMapBufferRange(GL_ARRAY_BUFFER, 0, text_quad_buff_size, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	/* force the data to be available */
	glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);

	/* update verts and tex coords */
	memcpy(old_data, debug_text_vert_buffer, text_quad_buff_size);

	glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, text_quad_buff_size);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	/* this projection is orthographic with left-top 0,0, right-bttom width, height coordinates */
	int location = glGetUniformLocation(debug_render_group.shader_info.program, "projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)debug_render_group.projection);

	location = glGetUniformLocation(debug_render_group.shader_info.program, "model");
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)ident);

	location = glGetUniformLocation(debug_render_group.shader_info.program, "view");
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)ident);

	/* draw all text */
	glDrawArrays(GL_TRIANGLES, 0, 6 * num_of_chars);

	fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	num_text_lines = 0;
	num_of_chars = 0;
	atX = 2;
	atY = HALF_FONT_SCALE + 2;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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


	int window_height = 720;
	int window_width = 1280;

	/* Create a fullscreen window */
	SDL_Window *screen = SDL_CreateWindow("OpenGL",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width, window_height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_OPENGL);

	SDL_SetRelativeMouseMode(1);

	/* initialize a render context, managed by SDL */
	SDL_GLContext *opengl_context = SDL_GL_CreateContext(screen);
	SDL_GL_SetSwapInterval(-1);

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
#if defined(__APPLE_CC__)
	err = err == GL_INVALID_ENUM ? 0 : err;
#endif
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

	struct sb_bitmap* font = make_bitmap_font(&debug_render_group.texture, BITMAP_WIDTH, BITMAP_HEIGHT);

	shader_compile(&debug_render_group.shader_info, "model", "shaders/simple_vert.glsl", "shaders/text_frag.glsl");
	shader_compile(&render_group.shader_info, "model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");

	glGenTextures(1, &render_group.texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render_group.texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 0);

	int x, y, comp;
	x = y = comp = 0;
	unsigned char* texture_data = stbi_load("Gray_granite_pxr128.png", &x, &y, &comp, 4);
	fprintf(stderr, "Image x: %d, y: %d, comps: %d\n", x, y, comp);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	mat4x4_identity(render_group.projection);

	float aspect = (float)window_width / (float)window_height;

	/* normal ortho projection from -1,1 is TL and 1.0,-1.0 is BR */
	mat4x4_perspective(render_group.projection, deg2rad(45.0f), aspect, 0.001f, 1000.0f);
	mat4x4_ortho(debug_render_group.projection, 0, window_width, window_height, 0.0, 1.0, -1.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &render_group.vao);
	glBindVertexArray(render_group.vao);

	int pos_attr = glGetAttribLocation(render_group.shader_info.program, "in_position");
	int uv_attr = glGetAttribLocation(render_group.shader_info.program, "in_tex");

	glGenBuffers(1, &render_group.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, render_group.vbo);
	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(pos_attr);

	glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(uv_attr);

	const float vertices[] =
	{
	/*   x     y     z	 s  t   */
		-0.15,  0.15,  0,  0, 0,
		 0.15,  0.15,  0,  1, 0,
		 0.15, -0.15,  0,  1, 1,

		 0.15, -0.15,  0,  1, 1,
		-0.15, -0.15,  0,  0, 1,
		-0.15,  0.15,  0,  0, 0
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &debug_render_group.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, debug_render_group.vbo);
	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(pos_attr);

	glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(uv_attr);
	glBufferData(GL_ARRAY_BUFFER, sizeof(debug_text_vert_buffer), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint32_t old = SDL_GetTicks();
	uint32_t now = SDL_GetTicks();
	int pause = 0;

	mat4x4 ident;
	/* translate quads by half of font scale to account for font scale */
	mat4x4_identity(ident);

	while (1) {
		sb_debug_cycle_begin_timed_block(UPDATE_AND_RENDER);
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
		sb_debug_cycle_end_timed_block(UPDATE_AND_RENDER);
		sb_debug_overlay_cycle_counters(font);

		/* render debug text */
		debug_draw();

		SDL_GL_SwapWindow(screen);
	}

	sb_bitmap_free(font);

	glDeleteBuffers(1, &render_group.vbo);
	glDeleteBuffers(1, &debug_render_group.vbo);
	glDeleteBuffers(1, &debug_render_group.texture);
	glDeleteVertexArrays(1, &render_group.vao);
	shader_cleanup(&render_group.shader_info);

	SDL_GL_DeleteContext(opengl_context);
	SDL_DestroyWindow(screen);
	SDL_Quit();

	return 0;
}
