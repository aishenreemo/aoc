#include <libcollections/vec.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"

enum cube_variant_t {
	UNKNOWN,
	RED,
	GREEN,
	BLUE,
};

struct cube_t {
	enum cube_variant_t variant;
	int count;
};

struct game_t {
	int id;
};

struct line_t {
	struct game_t game;
	struct vec_t cubes;
};


bool is_cube_possible(struct cube_t const *cube);

void expect_line(struct line_t *line, char const *input, int *index);
void expect_cube(struct cube_t *cube, char const *input, int *index);
void expect_game(struct game_t *game, char const *input, int *index);
void expect_number(int *number, char const *input, int *index);


int main() {
	char const *input = load_file("2023/input_02.txt");
	uint const input_len = strlen(input);

	struct vec_t lines;
	vec_init(&lines, sizeof(struct line_t));

	int index = 0;
	while (index < input_len) {
		struct line_t *line = malloc(sizeof(struct line_t));
		vec_init(&line->cubes, sizeof(struct cube_t));
		expect_line(line, input, &index);
		vec_push(&lines, line);
	}

	uint part_1_total = 0;
	uint part_2_total = 0;
	for (int i = 0; i < lines.length; i++) {
		struct line_t *line = vec_get(&lines, i);
		printf("id: \e[32m%d\e[0m\t", line->game.id);

		bool part_1_is_possible = false;
		uint part_2_maximum_cubes[3] = { 0, 0, 0 };

		for (int j = 0; j < line->cubes.length; j++) {
			struct cube_t *cube = vec_get(&line->cubes, j);
			printf("\e[34m%d\e[0mx\e[33m%d\e[0m, ", cube->variant, cube->count);

			part_1_is_possible = is_cube_possible(cube);
			if (!part_1_is_possible) break;
		}

		for (int j = 0; j < line->cubes.length; j++) {
			struct cube_t *cube = vec_get(&line->cubes, j);
			if (cube->count <= part_2_maximum_cubes[cube->variant - 1]) continue;

			part_2_maximum_cubes[cube->variant - 1] = cube->count;
		}

		if (part_1_is_possible) {
			part_1_total += line->game.id;
		}

		part_2_total += part_2_maximum_cubes[0] *
			part_2_maximum_cubes[1] *
			part_2_maximum_cubes[2];

		printf("\n");
	}

	printf("part 1 total: %d\n", part_1_total);
	printf("part 2 total: %d\n", part_2_total);

	for (int i = 0; i < lines.length; i++) {
		struct line_t *line = vec_get(&lines, i);
		vec_drop(&line->cubes);
	}

	vec_drop(&lines);

	return EXIT_SUCCESS;
}

bool is_cube_possible(struct cube_t const *cube) {
	if (cube->variant == RED) {
		return cube->count <= 12;
	} else if (cube->variant == GREEN) {
		return cube->count <= 13;
	} else if (cube->variant == BLUE) {
		return cube->count <= 14;
	}

	return false;
}


void expect_line(struct line_t *line, char const *input, int *index) {
	expect_game(&line->game, input, index);

	while (input[*index] != '\n' && input[*index] != '\0') {
		if (isdigit(input[*index])) {
			struct cube_t *cube = malloc(sizeof(struct cube_t));
			expect_cube(cube, input, index);
			vec_push(&line->cubes, cube);
		} else {
			*index += 1;
		}
	}

	*index += 1;
}


void expect_game(struct game_t *game, char const *input, int *index) {
	while (input[*index] != ':') {
		if (isdigit(input[*index])) {
			expect_number(&game->id, input, index);
			break;
		} else {
			*index += 1;
		}
	}

	*index += 1;
}


void expect_cube(struct cube_t *cube, char const *input, int *index) {
	expect_number(&cube->count, input, index);

	while (
		input[*index] != ',' &&
		input[*index] != ';' &&
		input[*index] != '\n' &&
		input[*index] != '\0'
	) {
		if (strncmp(input + *index, "red", strlen("red")) == 0) {
			cube->variant = RED;
			*index += strlen("red");
			break;
		} else if (strncmp(input + *index, "blue", strlen("blue")) == 0) {
			cube->variant = BLUE;
			*index += strlen("BLUE");
			break;
		} else if (strncmp(input + *index, "green", strlen("green")) == 0) {
			cube->variant = GREEN;
			*index += strlen("green");
			break;
		}

		*index += 1;
	}
}


void expect_number(int *number, char const *input, int *index) {
	int start_index = *index;
	while (isdigit(input[*index])) {
		*index += 1;
	}

	int length = *index - start_index;
	char substring[length + 1];
	strncpy(substring, input + start_index, length);
	substring[length] = '\0';

	*number = atoi(substring);
}
