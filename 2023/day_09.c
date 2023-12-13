#include <libcollections/vec.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "macros.h"


static struct vec_t lines;

static char *input;
static int input_length;
static int cursor;


void expect_numbers(struct vec_t *line);
void expect_number(int64_t *number);

int64_t extrapolate(struct vec_t *line, bool is_part_2);


int main(void) {
	cursor = 0;
	input = load_file("2023/input_09.txt");
	input_length = strlen(input);

	vec_init(&lines, sizeof(struct vec_t));

	while (cursor < input_length) {
		struct vec_t line;
		vec_init(&line, sizeof(int64_t));
		expect_numbers(&line);
		vec_push(&lines, &line);
	}

	int part_1_total = 0;
	int part_2_total = 0;
	for (int i = 0; i < lines.length; i++) {
		struct vec_t const *line = vec_get(&lines, i);
		part_1_total += extrapolate((struct vec_t *) line, false);
		part_2_total += extrapolate((struct vec_t *) line, true);
	}

	printf("part 1: %d\n", part_1_total);
	printf("part 2: %d\n", part_2_total);

	for (int i = 0; i < lines.length; i++) {
		struct vec_t const *line = vec_get(&lines, i);
		vec_drop((void *) line);
	}

	vec_drop(&lines);
	free(input);

	return EXIT_SUCCESS;
}


int64_t extrapolate(struct vec_t *line, bool is_part_2) {
	int64_t out = 0;
	int64_t mul = -1;

	struct vec_t vec;
	vec_init(&vec, sizeof(int64_t));
	vec_copy(&vec, line);

	if (is_part_2) {
		out += * (int64_t const *) vec_get(&vec, 0);
	}

	while (true) {
		bool all_zero = true;
		for (int i = 0; i < vec.length; i++) {
			int64_t const *num = vec_get(&vec, i);
			if (*num == 0) continue;
			all_zero = false;
			break;
		}

		if (all_zero) break;

		for (int i = 1; i < vec.length; i++) {
			int64_t *num_1 = (int64_t *) vec_get(&vec, i - 1);
			int64_t const *num_2 = vec_get(&vec, i);
			*num_1 = *num_2 - *num_1;
		}

		if (!is_part_2) {
			out += * (int64_t const *) vec_get(&vec, vec.length - 1);
			vec_remove(&vec, vec.length - 1);
		} else {
			out += * (int64_t const *) vec_get(&vec, 0) * mul;
			mul *= -1;
			vec_remove(&vec, vec.length - 1);
		}
	}

	vec_drop(&vec);

	return out;
}


void expect_numbers(struct vec_t *line) {
	while (cursor < input_length && input[cursor] != '\n') {
		if (isdigit(input[cursor]) || input[cursor] == '-') {
			int64_t number;
			expect_number(&number);
			vec_push(line, &number);
		} else {
			cursor += 1;
		}
	}

	cursor += 1;
}


void expect_number(int64_t *number) {
	int start = cursor;

	if (input[cursor] == '-') {
		cursor += 1;
	}

	while (isdigit(input[cursor])) {
		cursor += 1;
	}

	int length = cursor - start;
	char substring[length + 1];

	strncpy(substring, input + start, length);
	substring[length] = '\0';

	*number = atoi(substring);
}
