#include <libcollections/vec.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

#include "macros.h"


static struct vec_t times;
static struct vec_t distances;
static uint64_t concatenated_time;
static uint64_t concatenated_distance;

static char *input;
static int input_length;
static int cursor;


void expect_times();
void expect_distances();
void expect_number(int *number);


int main(void) {
	cursor = 0;
	input = load_file("2023/input_06.txt");
	input_length = strlen(input);

	concatenated_time = 0;
	concatenated_distance = 0;

	vec_init(&times, sizeof(int));
	vec_init(&distances, sizeof(int));

	expect_times();
	expect_distances();

	int margin_of_error = 1;
	for (int i = 0; i < times.length; i++) {
		int *time = vec_get(&times, i);
		int *distance = vec_get(&distances, i);

		int count = 0;
		for (int j = 1; j < *time; j++) {
			int distance_1 = (*time - j) * j;
			if (distance_1 < *distance) continue;
			count += 1;
		}

		concatenated_time *= pow(10, (int) log10(*time) + 1);
		concatenated_distance *= pow(10, (int) log10(*distance) + 1);
		concatenated_time += *time;
		concatenated_distance += *distance;

		if (count <= 0) continue;
		margin_of_error *= count;
	}

	uint64_t margin_of_error_1 = 0;
	for (int j = 1; j < concatenated_time; j++) {
		uint64_t distance = (concatenated_time - j) * j;
		if (distance < concatenated_distance) continue;
		margin_of_error_1 += 1;
	}

	printf("part 1: %d\n", margin_of_error);
	printf("part 2: %ld\n", margin_of_error_1);

	vec_drop(&times);
	vec_drop(&distances);
	free(input);

	return EXIT_SUCCESS;
}


void expect_times() {
	while (input[cursor] != '\n') {
		if (isdigit(input[cursor])) {
			int number;
			expect_number(&number);
			vec_push(&times, &number);
			continue;
		}

		cursor += 1;
	}

	cursor += 1;
}


void expect_distances() {
	while (cursor < input_length) {
		if (isdigit(input[cursor])) {
			int number;
			expect_number(&number);
			vec_push(&distances, &number);
		}

		cursor += 1;
	}

	cursor += 1;
}


void expect_number(int *number) {
	int start = cursor;
	while (isdigit(input[cursor])) {
		cursor += 1;
	}

	int length = cursor - start;
	char substring[length + 1];

	strncpy(substring, input + start, length);
	substring[length] = '\0';

	*number = atoi(substring);
}

