#pragma once

typedef struct s_shader {
	int program;
	char program_name[MAX_PATH];
} shader;

shader *make_shader(const char* name, const char* vert_path, const char* frag_path);
void shader_compile(shader * s, const char * name, const char * vertex_path, const char * frag_path);
void shader_cleanup(shader* s);
void shader_use(shader* s);