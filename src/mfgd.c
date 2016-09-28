/*  
 *	TODO:
 *  - render_group list for rendering
 *  - multiple rooms - render multiple of them if the screen is overlapping them
 *  - collision detection 
 *  - walkable tiles? 
 *	VISITED (not sure if done):
 *	- Draw colored (vertex) quads tiled across the screen
 *	- include some spacing between tiles
 *	- Bitmap based font rendering
 *	- easy way to accumulate text that's spit out at the end of a frame 
 */

#if defined(_WIN32) 
#include <Windows.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif

#include <glew.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <SDL.h>
#undef main

#include "linmath.h"
#include "utils.h"
#include "shader.h"
#include "sb_debug.h"

static struct sb_memory_arena global_memory;

struct sb_render_group {
	int screen_width;
	int screen_height;
	shader shader_info;
	int vao;
	int vbo;
	int ibo;
	int texture;
	mat4x4 projection;
};

static struct sb_render_group main_render_group = { 0 };
static struct sb_render_group debug_render_group = { 0 };

#define TILE_SIZE 60
#define TILE_PAD 1.00
#define TILEMAP_WIDTH 20
#define TILEMAP_HEIGHT 10

struct world {
	float tile_side_in_meters;
	float tile_side_in_pixels;
	float tile_side_meters_to_pixels;
	
	int32_t count_x;
	int32_t count_y;

	int32_t lower_left_x;
	int32_t lower_left_y;

	int32_t padding;

	int32_t tile_map_count_x;
	int32_t tile_map_count_y;
	
	struct tile_map* tile_maps;
	struct tile_map_gl_buffer* gl_data;
};

static struct world global_world = { 0 };

struct world_position {
	uint32_t tile_map_x;
	uint32_t tile_map_y;

	uint32_t tile_x;
	uint32_t tile_y;

	vec2 tile_offset;
};

struct tile_map_gl_buffer {
	float* vert_buffer;
	unsigned int* index_buffer;
	uint32_t count;
	uint32_t used;
	uint32_t index_count;
	uint32_t stride;
};

struct game_state {
	struct world_position player_pos;
	vec2 player_vel;
	float frame_dt;
};

static struct game_state global_game_state = { 0 };

static struct tile_map_gl_buffer* make_tile_map_vert_buffer(struct sb_memory_arena* arena, int num_x, int num_y) {
	/* total size in bytes needed to hold a tile map vertex buffer */
	uint8 quad_comp_count = 8 * 4; /* xyz rgb st * 4 */
	uint32_t vert_buff_size = sizeof(float) * num_x * num_y * quad_comp_count;

	struct tile_map_gl_buffer* tm = (struct tile_map_gl_buffer*)sb_push_size(arena, struct tile_map_gl_buffer);

	tm->vert_buffer = (float*)sb_push_array(arena, num_x * num_y * quad_comp_count, float);
	tm->count = num_x * num_y * quad_comp_count;
	tm->used = 0;
	tm->stride = 8;

	/* push enough for two tris per x,y */
	tm->index_buffer = (uint*)sb_push_array(arena, num_x * num_y * 6, uint);
	tm->index_count = 0;
	return tm;
}

static void build_tile_map_vert_buffer(struct tile_map_gl_buffer* tmvb, struct sb_render_group* render_group, int tile_size, int num_x, int num_y) {
	float top_left_x = 0;
	float top_left_y = render_group->screen_height - tile_size;

	for (int y = 0; y < num_y; ++y) {
		for (int x = 0; x < num_x; ++x) {
			
			/* start at top left */
			float *vx = (tmvb->vert_buffer + tmvb->used);
			float *vy = vx + 1;
			float *vz = vy + 1;
			float *vr = vz + 1;
			float *vg = vr + 1;
			float *vb = vg + 1;
			float *vs = vb + 1;
			float *vt = vs + 1;

			*vx = top_left_x + (x * tile_size * TILE_PAD);
			*vy = top_left_y - (y * tile_size * TILE_PAD);
			*vz = 0.0f;

			*vr = 1.0f;
			*vg = 0.0f;
			*vb = 0.0f;
			
			*vs = -1.0f;
			*vt = 1.0f;

			/* bottom left */
			vx = vt + 1;
			vy = vx + 1;
			vz = vy + 1;
			vr = vz + 1;
			vg = vr + 1;
			vb = vg + 1;
			vs = vb + 1;
			vt = vs + 1;

			*vx = top_left_x + (x * tile_size * TILE_PAD);
			*vy = top_left_y - (y * tile_size * TILE_PAD) + tile_size;
			*vz = 0.0f;

			*vr = 0.0f;
			*vg = 1.0f;
			*vb = 0.0f;

			*vs = -1.0f;
			*vt = -1.0f;

			/* bottom right */
			vx = vt + 1;
			vy = vx + 1;
			vz = vy + 1;
			vr = vz + 1;
			vg = vr + 1;
			vb = vg + 1;
			vs = vb + 1;
			vt = vs + 1;

			*vx = top_left_x + (x * tile_size * TILE_PAD) + tile_size;
			*vy = top_left_y - (y * tile_size * TILE_PAD) + tile_size;
			*vz = 0.0f;

			*vr = 0.0f;
			*vg = 0.0f;
			*vb = 1.0f;

			*vs = 1.0f;
			*vt = -1.0f;

			/* top right */
			vx = vt + 1;
			vy = vx + 1;
			vz = vy + 1;
			vr = vz + 1;
			vg = vr + 1;
			vb = vg + 1;
			vs = vb + 1;
			vt = vs + 1;

			*vx = top_left_x + (x * tile_size * TILE_PAD) + tile_size;
			*vy = top_left_y - (y * tile_size * TILE_PAD);
			*vz = 0.0f;

			*vr = 1.0f;
			*vg = 1.0f;
			*vb = 1.0f;

			*vs = 1.0f;
			*vt = 1.0f;

			tmvb->used += 32;
		}
	}
}

static void build_tile_map_index_buffer(struct tile_map_gl_buffer* vb, struct sb_render_group* render_group, int num_x, int num_y) {
	unsigned int* index_buffer = vb->index_buffer;
	for (int i = 0; i < (num_y * num_x * 6); i+=4) {
		/* drawing order tl, bl, br, br, tr, tl */
		// [0, 1, 2, 2, 3, 0]
		*index_buffer++ = (0 + i);
		*index_buffer++ = (1 + i);
		*index_buffer++ = (2 + i);
		*index_buffer++ = (2 + i);
		*index_buffer++ = (3 + i);
		*index_buffer++ = (0 + i);
		vb->index_count += 6;
	}
}

/* helper for when verts are all the same color */
#define color_quad(vert_buff, color) color_quad_verts_vec3(vert_buff, color, color, color, color)

static void color_quad_verts_vec3(float* vert_buff, vec3 tl, vec3 bl, vec3 br, vec3 tr) {
	/* top left */
	{
		float* vr = vert_buff + 3, *vg = vert_buff + 4, *vb = vert_buff + 5;
		*vr = *tl++, *vg = *tl++, *vb = *tl++;
	}

	/* bottom left */
	{
		float* vr = vert_buff + 11, *vg = vert_buff + 12, *vb = vert_buff + 13;
		*vr = *bl++, *vg = *bl++, *vb = *bl++;
	}

	/* bottom right */
	{
		float* vr = vert_buff + 19, *vg = vert_buff + 20, *vb = vert_buff + 21;
		*vr = *br++, *vg = *br++, *vb = *br++;
	}

	/* bottom right */
	{
		float* vr = vert_buff + 27, *vg = vert_buff + 28, *vb = vert_buff + 29;
		*vr = *tr++, *vg = *tr++, *vb = *tr++;
	}
}

struct tile_map {
	uint32_t* tiles;
};

static void draw_tile_map(struct sb_render_group* render_group, int num_tiles) {
	struct tile_map* tilemap = global_world.tile_maps + (global_game_state.player_pos.tile_map_y * global_world.tile_map_count_x  + global_game_state.player_pos.tile_map_x);
	uint32_t* tiles = tilemap->tiles;
	uint32_t gl_index = 0;

	for (int y = 0; y < global_world.count_y; ++y) {
		for (int x = 0; x < global_world.count_x; ++x) {
			uint32_t tile = *tiles++;

			float* vert_buff = global_world.gl_data->vert_buffer + gl_index;

			/* player occupied tile */
			if (global_game_state.player_pos.tile_x == x && (global_world.count_y - 1 - global_game_state.player_pos.tile_y) == y) {
				vec3 black = { 0.0, 0.0, 0.0 };
				color_quad(vert_buff, black);
			}
			else if (tile == 1) {
				vec3 gray = { 1, 1, 1 };
				color_quad(vert_buff, gray);
			}
			else {
				vec3 tl = { 1.0f, 0.0f, 0.0f };
				vec3 bl = { 0.0f, 1.0f, 0.0f };
				vec3 br = { 0.0f, 0.0f, 1.0f };
				vec3 tr = { 0.0f, 0.0f, 0.0f };
				color_quad_verts_vec3(vert_buff, tl, bl, br, tr);
			}

			/* move by quad */
			gl_index += global_world.gl_data->stride * 4;
		}
	}

	int location = glGetUniformLocation(render_group->shader_info.program, "model");
	mat4x4 model; mat4x4_identity(model);
	mat4x4 ident; mat4x4_identity(model);

	mat4x4_translate_in_place(model, 0.0f, -global_world.tile_side_in_pixels, 0.0);

	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)model);

	/* 6 indices to draw a quad */
	glBufferSubData(GL_ARRAY_BUFFER, 0, global_world.gl_data->used * sizeof(float), global_world.gl_data->vert_buffer);
	glDrawElements(GL_TRIANGLES, num_tiles * 6, GL_UNSIGNED_INT, 0);
}

static canonicalize_position(struct world_position*out, const struct world_position* pos) {
	out->tile_map_x = pos->tile_map_x;
	out->tile_map_y = pos->tile_map_y;

	out->tile_x = pos->tile_x;
	out->tile_y = pos->tile_y;

	vec2_dup(out->tile_offset, pos->tile_offset);

	if (out->tile_offset[0] > (global_world.tile_side_in_meters * 0.85)) {
		out->tile_x++;
		out->tile_offset[0] = 0.0f;
	}
		   
	if (out->tile_offset[0] < -(global_world.tile_side_in_meters * 0.85)) {
		out->tile_x--;
		out->tile_offset[0] = 0.0f;
	}	   
		   
	if (out->tile_offset[1] < -global_world.tile_side_in_meters * 0.85) {
		out->tile_y--;
		out->tile_offset[1] = 0.0f;
	}	   
		   
	if (out->tile_offset[1] > (global_world.tile_side_in_meters * 0.85)) {
		out->tile_y++;
		out->tile_offset[1] = 0.0f;
	}	   

	/* pull out and use velocity (dx vs dy) to determine how to move */	   
	if (out->tile_x > global_world.count_x - 1) {
		out->tile_map_x++;
		out->tile_x = 0;
	}	   
		   
	if (out->tile_x < 0) {
		out->tile_map_x--;
		out->tile_x = global_world.count_x - 1;
	}	   
		   
	if (out->tile_y > global_world.count_y - 1) {
		out->tile_map_y++;
		out->tile_y = global_world.count_y - 1;
	}	   
		   
	if (out->tile_y < 0) {
		out->tile_map_y--;
		out->tile_y = 0.0;
	}	   
		   
	if (out->tile_map_y < 0) {
		out->tile_map_y = (global_world.tile_map_count_y - 1);
	}	   
		   
	if (out->tile_map_y >(global_world.tile_map_count_y - 1)) {
		out->tile_map_y = 0;
	}
}

static double square(double v) {
	return v * v;
}

static void update(float dt) {
}

static void draw_player(struct sb_render_group* render_group) {
	int tile_x = global_game_state.player_pos.tile_x;
	int tile_y = global_game_state.player_pos.tile_y;

	int location = glGetUniformLocation(render_group->shader_info.program, "model");
	mat4x4 model; mat4x4_identity(model);

	mat4x4_translate_in_place(model, (tile_x * global_world.tile_side_in_pixels) + (global_game_state.player_pos.tile_offset[0] * global_world.tile_side_meters_to_pixels), 
									-(global_world.tile_side_in_pixels * global_world.count_y) + (tile_y * global_world.tile_side_in_pixels) + (global_game_state.player_pos.tile_offset[1] * global_world.tile_side_meters_to_pixels), 0.0);

	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)model);

	vec3 player_color = { 1.0, 1.0, 0.0 };
	color_quad(global_world.gl_data->vert_buffer, player_color);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * 4 * sizeof(float), global_world.gl_data->vert_buffer);
	
	/* 6 indices to draw a quad */
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)(0));
}

static void my_draw() {
	sb_debug_cycle_begin_timed_block(RENDER_GROUP_RENDER);
	glClearColor(210.f / 255.f, 230.f / 255.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_use(&main_render_group.shader_info);
	glBindBuffer(GL_ARRAY_BUFFER, main_render_group.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, main_render_group.ibo);

	glBindTexture(GL_TEXTURE_2D, main_render_group.texture);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	int location = glGetUniformLocation(main_render_group.shader_info.program, "projection");
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)main_render_group.projection);

	location = glGetUniformLocation(main_render_group.shader_info.program, "view");
	mat4x4 view;  mat4x4_identity(view);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)view);

	draw_tile_map(&main_render_group, TILEMAP_WIDTH * TILEMAP_HEIGHT);

	draw_player(&main_render_group);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	sb_debug_cycle_end_timed_block(RENDER_GROUP_RENDER);
}

GLsync fence = 0;
static void debug_draw() {
	mat4x4 ident; mat4x4_identity(ident);
	glUseProgram(debug_render_group.shader_info.program);
	glBindBuffer(GL_ARRAY_BUFFER, debug_render_group.vbo);
	glBindTexture(GL_TEXTURE_2D, debug_render_group.texture);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

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
	/* 4GB in bytes */
	size_t alloc_size = 4294967296;
	sb_memory_arena_init(&global_memory, alloc_size);

	global_world.count_x = TILEMAP_WIDTH;
	global_world.count_y = TILEMAP_HEIGHT;
	global_world.padding = 1.0f; /* percentage of tile width / height */
	global_world.tile_side_in_pixels = TILE_SIZE;
	global_world.tile_side_in_meters = 2.0;
	global_world.tile_side_meters_to_pixels = (float)global_world.tile_side_in_pixels / (float)global_world.tile_side_in_meters;
	global_world.tile_map_count_x = 2;
	global_world.tile_map_count_y = 2;

	uint32_t tiles00[TILEMAP_HEIGHT][TILEMAP_WIDTH] = {
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};

	uint32_t tiles01[TILEMAP_HEIGHT][TILEMAP_WIDTH] = {
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};

	uint32_t tiles10[TILEMAP_HEIGHT][TILEMAP_WIDTH] = {
		{ 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};

	uint32_t tiles11[TILEMAP_HEIGHT][TILEMAP_WIDTH] = {
		{ 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};

	struct tile_map maps[2][2] = { 0 };

	maps[0][0].tiles = tiles00;
	maps[0][1].tiles = tiles01;
	maps[1][0].tiles = tiles10;
	maps[1][1].tiles = tiles11;

	global_world.tile_maps = maps;

	global_game_state.player_pos.tile_map_x = 0;
	global_game_state.player_pos.tile_map_y = 0;

	global_game_state.player_pos.tile_x = 1;
	global_game_state.player_pos.tile_y = 1;

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

	global_world.lower_left_y = 0;
	global_world.lower_left_x = 0;

	struct sb_bitmap* font = sb_debug_make_bitmap_font(&debug_render_group.texture, BITMAP_WIDTH, BITMAP_HEIGHT);

	shader_compile(&debug_render_group.shader_info, "model", "shaders/simple_vert.glsl", "shaders/text_frag.glsl");
	shader_compile(&main_render_group.shader_info, "model", "shaders/simple_vert.glsl", "shaders/simple_frag.glsl");

	glGenTextures(1, &main_render_group.texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, main_render_group.texture);

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

	mat4x4_identity(main_render_group.projection);

	float aspect = (float)window_width / (float)window_height;

	/* normal ortho projection from -1,1 is TL and 1.0,-1.0 is BR */
	//mat4x4_perspective(render_group.projection, deg2rad(45.0f), aspect, 0.001f, 1000.0f);
	mat4x4_ortho(main_render_group.projection, 0.0, window_width, 0.0, window_height, 1.0, -1.0);
	mat4x4_ortho(debug_render_group.projection, 0, window_width, window_height, 0.0, 1.0, -1.0);

	main_render_group.screen_height = window_height;
	main_render_group.screen_width = window_width;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &main_render_group.vao);
	glBindVertexArray(main_render_group.vao);

	int pos_attr = glGetAttribLocation(main_render_group.shader_info.program, "in_position");
	int color_attr = glGetAttribLocation(main_render_group.shader_info.program, "in_color");
	int uv_attr = glGetAttribLocation(main_render_group.shader_info.program, "in_tex");

	glGenBuffers(1, &main_render_group.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, main_render_group.vbo);
	glGenBuffers(1, &main_render_group.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, main_render_group.ibo);

	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(pos_attr);

	glVertexAttribPointer(color_attr, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(color_attr);

	glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(uv_attr);

	struct tile_map_gl_buffer* tm = make_tile_map_vert_buffer(&global_memory, TILEMAP_WIDTH, TILEMAP_HEIGHT);
	build_tile_map_vert_buffer(tm, &main_render_group, TILE_SIZE, TILEMAP_WIDTH, TILEMAP_HEIGHT);
	build_tile_map_index_buffer(tm, &main_render_group, TILEMAP_WIDTH, TILEMAP_HEIGHT);

	global_world.gl_data = tm;

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tm->count, NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * TILEMAP_HEIGHT * TILEMAP_WIDTH * 6, tm->index_buffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &debug_render_group.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, debug_render_group.vbo);
	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(pos_attr);

	glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(uv_attr);
	glBufferData(GL_ARRAY_BUFFER, sizeof(debug_text_vert_buffer), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint64_t old = SDL_GetPerformanceCounter();
	uint64_t now = SDL_GetPerformanceCounter();
	int pause = 0;

	mat4x4 ident;
	/* translate quads by half of font scale to account for font scale */
	mat4x4_identity(ident);

	while (1) {
		sb_debug_cycle_begin_timed_block(UPDATE_AND_RENDER);
		SDL_Event event;
		int quit = 0;
		int rel_x, rel_y;

		if (!pause) {
			old = now;
			now = SDL_GetPerformanceCounter();
		}

		double dt = (double)(now - old);

		float dt_seconds = dt / SDL_GetPerformanceFrequency();
		if (dt_seconds > 0.016f) dt_seconds = 0.016f;
		//if (dt_seconds < 0.008f) dt_seconds = 0.008f;

		char debug_string[256] = { 0 };
		sprintf_s(debug_string, 256, "Frame: %.3fms", dt_seconds * 1000.0);
		sb_debug_render_text(debug_string, font);

		sprintf_s(debug_string, 256, "TileMap: %d,%d, Player Pos: %d,%d Tile Offset: %.3f,%.3f Player Vel: %f", 
			global_game_state.player_pos.tile_map_x, global_game_state.player_pos.tile_map_y,
			global_game_state.player_pos.tile_x, global_game_state.player_pos.tile_y,
			global_game_state.player_pos.tile_offset[0], global_game_state.player_pos.tile_offset[1],
			vec2_len(global_game_state.player_vel));

		sb_debug_render_text(debug_string, font);

		vec2 accel = { 0 };
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN: {
				uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);
				if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP])    accel[1] =  1.0;
				if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN])  accel[1] = -1.0;
				if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) accel[0] =  1.0;
				if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT])  accel[0] = -1.0;
			}
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				break;
			case SDL_MOUSEMOTION:
				SDL_GetRelativeMouseState(&rel_x, &rel_y);
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			}
		}

		vec2 old_player_pos; vec2_dup(old_player_pos, global_game_state.player_pos.tile_offset);
		vec2 old_player_vel; vec2_dup(old_player_vel, global_game_state.player_vel);

		{
			float accel_speed = 5.0; // m/s^2
			vec2 temp_accel; vec2_dup(temp_accel, accel);
			if (accel[0] != 0.0 && accel[1] != 0.0)
				vec2_scale(accel, temp_accel, 0.70718678);

			vec2_dup(temp_accel, accel);
			vec2_scale(accel, temp_accel, accel_speed);

			vec2 temp_vel; 
			vec2_scale(temp_vel, old_player_vel, -0.7);
			vec2_add(temp_accel, temp_vel, accel);
			vec2_dup(accel, temp_accel);
		}


		struct world_position temp_player_pos; canonicalize_position(&temp_player_pos, &global_game_state.player_pos);
		{
			/* p = 1/2adt^2 + vdt + p */
			float dtsqd = square(dt_seconds);
			vec2 temp;
			vec2_scale(temp, accel, 0.5);

			vec2 accel_term;
			vec2_scale(accel_term, temp, dtsqd);

			vec2 vel_term;
			vec2_scale(vel_term, old_player_vel, dt_seconds);

			vec2 new_pos;
			vec2_add(new_pos, old_player_pos, vel_term);

			vec2 temp_pos; vec2_dup(temp_pos, new_pos);
			vec2_add(temp_player_pos.tile_offset, temp_pos, accel_term);
		}

		{
			vec2 accel_term;
			vec2_scale(accel_term, accel, dt_seconds);

			vec2_add(global_game_state.player_vel, accel_term, old_player_vel);
		}

		struct world_position new_player_pos = { 0 };
		canonicalize_position(&new_player_pos, &temp_player_pos);

		global_game_state.player_pos.tile_offset[0] = new_player_pos.tile_offset[0];
		global_game_state.player_pos.tile_offset[1] = new_player_pos.tile_offset[1];
		global_game_state.player_pos.tile_x = new_player_pos.tile_x;
		global_game_state.player_pos.tile_y = new_player_pos.tile_y;

		if (quit == 1) // if received instruction to quit
			break;

		sb_debug_cycle_begin_timed_block(UPDATE);
		update(dt);
		sb_debug_cycle_end_timed_block(UPDATE);

		sb_debug_cycle_begin_timed_block(RENDER);
		my_draw();
		sb_debug_cycle_end_timed_block(RENDER);

		sb_debug_cycle_begin_timed_block(DEBUG_RENDER_GROUP_RENDER);

		/* render debug text */
		debug_draw();
		sb_debug_cycle_end_timed_block(DEBUG_RENDER_GROUP_RENDER);
		sb_debug_cycle_end_timed_block(UPDATE_AND_RENDER);

		glFinish();
		SDL_GL_SwapWindow(screen);
	}

	sb_bitmap_free(font);

	glDeleteBuffers(1, &main_render_group.vbo);
	glDeleteBuffers(1, &main_render_group.ibo);
	glDeleteBuffers(1, &debug_render_group.vbo);
	glDeleteBuffers(1, &debug_render_group.texture);
	glDeleteVertexArrays(1, &main_render_group.vao);
	shader_cleanup(&main_render_group.shader_info);

	SDL_GL_DeleteContext(opengl_context);
	SDL_DestroyWindow(screen);
	SDL_Quit();

	return 0;
}
