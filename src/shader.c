#include <stdlib.h>
#include <assert.h>
#if !defined(_WIN32)
#include <unistd.h>
#include <sys/mman.h>
#else
#include <Windows.h>
#define ssize_t size_t
#endif
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <glew.h>

#include "shader.h"
#include "utils.h"

#define STB
#include <STB.h>

shader *make_shader(const char* name, const char* vertex_path, const char* frag_path) {
	shader *s = (shader*)calloc(1, sizeof(*s));

	init_shader(s, name, vertex_path, frag_path);
	return s;
}

void init_shader(shader* s, const char* name, const char* vertex_path, const char* frag_path) {
	assert(s != NULL);

	/* cheat for now, copy later */
	strcpy_s(s->program_name, sizeof(s->program_name), name);
	strcpy_s(s->vertex_path, sizeof(s->vertex_path), vertex_path);
	strcpy_s(s->fragment_path, sizeof(s->fragment_path), frag_path);
}

void shader_cleanup(shader* s) {
	glDetachShader(s->program, s->vertex_shader);
	glDetachShader(s->program, s->fragment_shader);
	glDeleteShader(s->vertex_shader);
	glDeleteShader(s->fragment_shader);
	glDeleteProgram(s->program);
}

char *read_shader(const char* path, ssize_t *size) {
#if defined(_WIN32)
	char shader_path[MAX_PATH] = { 0 };

	char* cpath = _strdup(path);
	char* c = NULL;
	while (c = strchr(cpath, '/')) {
		*c = '\\';
	}

	_fullpath(shader_path, cpath, MAX_PATH);

	HANDLE file_handle = CreateFile(shader_path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file_handle == INVALID_HANDLE_VALUE) {
		long error = GetLastError();
		return NULL;
	}

	LARGE_INTEGER lsize;
	GetFileSizeEx(file_handle, &lsize);
	*size = lsize.LowPart;
	char* buffer = (char*)malloc(*size + 1);
	ReadFile(file_handle, buffer, (long)(*size), NULL, NULL);
	buffer[*size] = '\0';
	CloseHandle(file_handle);
	return buffer;
#else
	int fd = open(path, O_RDONLY);
	struct stat sb;
	char *addr = NULL;
	fstat(fd, &sb);
	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	*size = sb.st_size;
	return addr;
#endif
}

void free_shader(char *addr) {
#if defined(_WIN32)
	free(addr);
#else
	munmap(addr, size);
#endif
}

void shader_compile(shader *s) {
	assert(s != NULL);

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	ssize_t vert_size;
	char* vert_source = read_shader(s->vertex_path, &vert_size);

	assert(vert_source != NULL);
	glShaderSource(vert_shader, 1, (const GLchar**)&vert_source, NULL);
	glCompileShader(vert_shader);
	free_shader(vert_source);

	{
		int status;  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &status);

		if (status == GL_FALSE) {
			int bufferSize, cpySize; glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &bufferSize);
			char* buffer = (char*)malloc(bufferSize + 1);
			glGetShaderInfoLog(vert_shader, bufferSize, &cpySize, buffer);
			buffer[bufferSize] = '\0';
			fprintf(stderr, "The vertex shader info log:\n%s\n", buffer);
			fflush(stderr);
			free(buffer);
		}
	}

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	ssize_t frag_size;
	GLchar* frag_source = (GLchar*)read_shader(s->fragment_path, &frag_size);
	assert(frag_source != NULL);

	glShaderSource(frag_shader, 1, (const GLchar**)&frag_source, NULL);
	glCompileShader(frag_shader);
	free_shader(frag_source);

	{
		int status;  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &status);

		if (status == GL_FALSE) {
			int bufferSize, cpySize; glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &bufferSize);
			char* buffer = (char*)malloc(bufferSize + 1);
			glGetShaderInfoLog(frag_shader, bufferSize, &cpySize, buffer);
			buffer[bufferSize] = '\0';
			fprintf(stderr, "The frag shader info log:\n%s\n", buffer);
			fflush(stderr);
			free(buffer);
		}
	}

	GLuint program = glCreateProgram();

	/* Some ATI cards need this. */
	glBindAttribLocation(program, 0, "in_position");
	glBindAttribLocation(program, 1, "in_color");
	glBindFragDataLocation(program, 0, "outColor");
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	{
		int status;  glGetShaderiv(frag_shader, GL_LINK_STATUS, &status);

		if (status == GL_FALSE) {
			int bufferSize, cpySize; glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &bufferSize);
			char* buffer = (char*)malloc(bufferSize + 1);
			glGetShaderInfoLog(frag_shader, bufferSize, &cpySize, buffer);
			buffer[bufferSize] = '\0';
			fprintf(stderr, "The shader linker info log:\n%s\n", buffer);
			fflush(stderr);
			free(buffer);
		}
	}

	s->program = program;
	s->vertex_shader = vert_shader;
	s->fragment_shader = frag_shader;

	s->pos_attr = glGetAttribLocation(s->program, "in_position");
	s->color_attr = glGetAttribLocation(s->program, "in_color");
}
