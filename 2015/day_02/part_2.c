#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "load_file.h"

unsigned int bow_length(unsigned int l, unsigned int w, unsigned int h);
unsigned int wrapper_length(unsigned int l, unsigned int w, unsigned int h);

// same algorithm from part 1
int main() {
	char *input = load_file("2015/day_02/input.txt");
	char *line = strtok(input, "\n");

	unsigned int total = 0;

	while (line != NULL) {
		unsigned int l;
		unsigned int w;
		unsigned int h;

		if (sscanf(line, "%ux%ux%u", &l, &w, &h) != 3) {
			printf("Failed to parse input.\n%s\n", line);
			goto exit_l;
		}

		unsigned int bl = bow_length(l, w, h);
		unsigned int wl = wrapper_length(l, w, h);

		total += bl;
		total += wl;

		line = strtok(NULL, "\n");
	}

	printf("Elves should order %d feet of ribbon.\n", total);

	exit_l:
		free(input);
		return 0;
}

// The feet of ribbon required for the perfect bow is equal to the cubic
// feet of volume of the present.
unsigned int bow_length(unsigned int l, unsigned int w, unsigned int h) {
	return l * w * h;
}

// The ribbon required to wrap a present is the shortest distance around its
// sides, or the smallest perimeter of any one face.
unsigned int wrapper_length(unsigned int l, unsigned int w, unsigned int h) {
	int smallest_index = 0;
	unsigned int arr[3] = { l, w, h };
	unsigned int smallest = UINT_MAX;
	unsigned int second_smallest = UINT_MAX;

	// find the smallest side
	for (int i = 0; i < 3; i++) {
		if (arr[i] > smallest) continue;
		smallest = arr[i];
		smallest_index = i;
	}

	// find the second smallest side
	for (int i = 0; i < 3; i++) {
		if (arr[i] > second_smallest) continue;
		if (i == smallest_index) continue;
		second_smallest = arr[i];
	}

	// a + a + b + b
	return 2 * (smallest + second_smallest);
}
