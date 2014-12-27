#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <glew.h>

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
    glDetachShader(s->program, s->vertex_shader);
    glDetachShader(s->program, s->fragment_shader);
    glDeleteShader(s->vertex_shader);
    glDeleteShader(s->fragment_shader);
    glDeleteProgram(s->program);

    free(s);
    s = NULL;
}

char *read_shader(const char* path, ssize_t *size) {
    int fd = open(path, O_RDONLY);
    struct stat sb;
    char *addr = NULL;
    fstat(fd, &sb);
    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    *size = sb.st_size;
    return addr;
}

void free_shader(char *addr, ssize_t size) {
    munmap(addr, size);
}

void shader_compile(shader *s) {
    assert(s != NULL);

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    ssize_t vert_size;
    char* vert_source = read_shader(s->vertex_path, &vert_size);

    assert(vert_source != NULL);
    glShaderSource(vert_shader, 1, (const GLchar**)&vert_source, NULL);
    glCompileShader(vert_shader);

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    ssize_t frag_size;
    GLchar* frag_source = (GLchar*)read_shader(s->fragment_path, &frag_size);
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
    free_shader(frag_source, frag_size);
    free_shader(vert_source, vert_size);
}
