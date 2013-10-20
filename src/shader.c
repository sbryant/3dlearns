#include <GL/glew.h>
#include <stdlib.h>
#include <assert.h>

#include "shader.h"
#include "utils.h"

shader *make_shader(const char* name, const char* vertex_path, const char* frag_path) {
    shader *s = (shader*)calloc(1, sizeof(shader));

    /* cheat for now, copy later */
    s->program_name = (char *)name;
    s->vertex_path = (char *)vertex_path;
    s->fragment_path = (char *)frag_path;

    return s;
}

void shader_cleanup(shader* s) {
    /* TODO CLEAN UP */
}


void shader_compile(shader *s) {
    assert(s != NULL);

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    printf("Vert Path: %s\n", s->vertex_path);
    char* vert_source = read_shader(s->vertex_path);
    assert(vert_source != NULL);
    glShaderSource(vert_shader, 1, (const GLchar**)&vert_source, NULL);
    glCompileShader(vert_shader);

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* frag_source = (GLchar*)read_shader(s->fragment_path);
    assert(frag_source != NULL);

    glShaderSource(frag_shader, 1, (const GLchar**)&frag_source, NULL);
    glCompileShader(frag_shader);

    GLuint program = glCreateProgram();

    /* Some ATI cards need this. */
    glBindAttribLocation(program, 0, "position");
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    s->program = program;
    s->vertex_shader = vert_shader;
    s->fragment_shader = frag_shader;

    s->pos_attr = glGetAttribLocation( s->program, "position" );
    s->color_attr = glGetAttribLocation( s->program, "color" );

    glBindFragDataLocation( s->program, 0, "outColor" );

    /* clean up */
    free(frag_source);
    free(vert_source);
}
