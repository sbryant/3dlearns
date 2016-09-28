#pragma once
#include <assert.h>
#include <stddef.h>
#include "sb_bitmap.h"

struct sb_memory_arena {
	uint64_t size;
	uint64_t used;
	uint8_t* buffer;
};

void sb_memory_arena_init(struct sb_memory_arena* arena, uint64_t size) {
	arena->buffer = malloc(size);
	arena->size = size;
	arena->used = 0;
}

struct sb_memory_arena* sb_memory_arena_make(uint64_t size) {
	struct sb_memory_arena* arena = (struct sb_memory_arena*)(malloc(sizeof(struct sb_memory_arena)));

	sb_memory_arena_init(arena, size);

	return arena;
}

#define sb_push_size(arena, type) _sb_memory_arena_push_size(arena, sizeof(type))
#define sb_push_array(arena, count, type) _sb_memory_arena_push_size(arena, count * sizeof(type))

void* _sb_memory_arena_push_size(struct sb_memory_arena* arena, size_t size) {
	assert((size + arena->used) <= arena->size && "Not enough memory left in the arena");

	void* memory_address = arena->buffer + arena->used;

	arena->used += size;

	return memory_address;
}

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

/* debug performance counters as seen from handemade hero */
enum sb_debug_cycle_counter_group {
	SB_DEBUG_CYCLE_COUNTER__UPDATE_AND_RENDER,
	SB_DEBUG_CYCLE_COUNTER__UPDATE,
	SB_DEBUG_CYCLE_COUNTER__RENDER,
	SB_DEBUG_CYCLE_COUNTER__PROGRAM_SETUP,
	SB_DEBUG_CYCLE_COUNTER__RENDER_GROUP_RENDER,
	SB_DEBUG_CYCLE_COUNTER__DEBUG_RENDER_GROUP_RENDER,

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

static struct sb_bitmap* sb_debug_bitmap_font(int width, int height) {
	const char* font_file_path = "C:/Windows/Fonts/cour.ttf";
	struct sb_debug_file_read_result* font_file = debug_read_entire_file(font_file_path);
	uint8_t* data = (uint8_t*)(font_file->data);

	struct sb_bitmap* bitmap = sb_bitmap_font(width, height, data);

	return bitmap;
}

struct sb_bitmap* sb_debug_make_bitmap_font(int* texture, int width, int height) {
	assert(texture != NULL && width > 0 && height > 0);
	glGenTextures(1, texture);

	/* generate bitmap font texture */
	struct sb_bitmap* font = sb_debug_bitmap_font(width, height);
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
		"Update",
		"Render",
		"Program Start",
		"Rendergroup Render",
		"Debug Rendergroup Render"
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