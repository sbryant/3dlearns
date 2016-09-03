#pragma once

typedef struct s_shader {
    int pos_attr;
    int color_attr;
    int program;
    int vertex_shader;
    int fragment_shader;
    char program_name[MAX_PATH];
    char vertex_path[MAX_PATH];
    char fragment_path[MAX_PATH];
} shader;

shader *make_shader(const char* name, const char* vert_path, const char* frag_path);
void init_shader(shader * s, const char * name, const char * vertex_path, const char * frag_path);
void shader_compile(shader* s);
void shader_cleanup(shader* s);
