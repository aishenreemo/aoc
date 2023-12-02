#ifndef __MACROS_H__
#define __MACROS_H__

#include <stdlib.h>
#include <stdio.h>

extern inline char *load_file(const char *path) {
	FILE *f = fopen(path, "rb");

	if (!f) {
		perror(path);
		exit(1);
	}

	fseek(f, 0L, SEEK_END);
	long length = ftell(f);
	rewind(f);

	char *out = (char *) calloc(sizeof(char), length + 1);

	if (!out) {
		printf("Failed to allocate memory.");
		fclose(f);
		exit(1);
	}

	if (fread(out, length, 1, f) != 1) {
		fclose(f);
		free(out);
		printf("Failed to read file.");
		exit(1);
	}

	return out;
}

#endif // __MACROS_H__
