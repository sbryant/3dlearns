#pragma once

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
