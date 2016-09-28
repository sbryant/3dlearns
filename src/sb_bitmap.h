#pragma once
#if defined(_WIN32) 
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif
#include <stdint.h>

#define STB_DEFINE 
#define STBI_PNG_ONLY
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "stb.h"
#include "stb_image.h"
#include "stb_truetype.h"

#define BITMAP_WIDTH 512
#define BITMAP_HEIGHT 512
#define BITMAP_BYTES_PER_PIXEL 1

#define FONT_SCALE 16
#define HALF_FONT_SCALE (FONT_SCALE / 2.0)

struct sb_bitmap {
	void* data;
	int width, height;
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

	int bitmap_size = height * width * BITMAP_BYTES_PER_PIXEL;

	if (zero == 1)
		bitmap->data = (void*)calloc(1, bitmap_size);
	else
		bitmap->data = (void*)malloc(bitmap_size);

	return bitmap;
}

static stbtt_bakedchar cdata[128];

extern struct sb_debug_file_read_result;

static struct sb_bitmap* sb_bitmap_font(int width, int height, void* data) {
	struct sb_bitmap* bitmap = make_empty_bitmap(width, height, 1);

	/* gets all characters from SPC - ~ (includes a-Z 0-9) */
	stbtt_BakeFontBitmap(data, 0, FONT_SCALE, bitmap->data, bitmap->width, bitmap->height, ' ', ('~' - ' ') + 1, &cdata[0]);

	return bitmap;
}

static void sb_bitmap_free(struct sb_bitmap* font) {
	assert(font != NULL && font->data != NULL && "Font and Font data need to be valid pointers");
	free(font->data);
	free(font);
}