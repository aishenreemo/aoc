#include <libcollections/vec.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>

#include "macros.h"


enum pipe_variant_t {
	PIPE_GROUND = 0x1 << 0,
	PIPE_VERTICAL = 0x1 << 1,
	PIPE_HORIZONTAL = 0x1 << 2,
	PIPE_DOWN_RIGHT = 0x1 << 3,
	PIPE_DOWN_LEFT = 0x1 << 4,
	PIPE_UP_RIGHT = 0x1 << 5,
	PIPE_UP_LEFT = 0x1 << 6,
	PIPE_STARTING = 0x1 << 7,
};

enum direction_t {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	UNKNOWN,
};

struct pipe_t {
	enum pipe_variant_t variant;
	int x;
	int y;
};

struct way_t {
	enum direction_t direction;
	struct pipe_t *pipe;
	struct way_t *next;
};


static char *input;
static int input_length;
static int cursor;

static int map_pos[2];
static int map_size[2];

static int offsets[4][3] = {
	{ 0, -1, PIPE_UP_LEFT | PIPE_UP_RIGHT | PIPE_HORIZONTAL},
	{ 1,  0, PIPE_DOWN_RIGHT | PIPE_UP_RIGHT | PIPE_VERTICAL},
	{ 0,  1, PIPE_DOWN_LEFT | PIPE_DOWN_RIGHT | PIPE_HORIZONTAL},
	{-1,  0, PIPE_DOWN_LEFT | PIPE_UP_LEFT | PIPE_VERTICAL},
};

static struct pipe_t starting_point;
static struct vec_t pipes;
static struct vec_t coordinates;


struct pipe_t *get_pipe_by_pos(int x, int y);
enum direction_t get_direction(enum direction_t, enum pipe_variant_t);

void get_map_size();
void expect_pipes();
void expect_pipe(struct pipe_t *);


int main() {
	cursor = 0;
	input = load_file("2023/input_10.txt");
	input_length = strlen(input);

	memset(map_pos, 0, sizeof(int) * 2);
	memset(map_size, 0, sizeof(int) * 2);

	vec_init(&pipes, sizeof(struct pipe_t));
	vec_init(&coordinates, sizeof(int) * 2);

	expect_pipes();
	get_map_size();


	// find an adjacent point that connects to the starting point
	struct way_t *way = malloc(sizeof(struct way_t));
	for (int i = 0; i < 4; i++) {
		int pos[2] = {
			offsets[i][0] + starting_point.x,
			offsets[i][1] + starting_point.y,
		};

		struct pipe_t *adjacent_pipe = get_pipe_by_pos(pos[0], pos[1]);

		if (adjacent_pipe == NULL) continue;
		if (adjacent_pipe->variant & offsets[i][2]) continue;

		way->direction = i;
		way->pipe = &starting_point;
		way->next = malloc(sizeof(struct way_t));
		way->next->pipe = adjacent_pipe;
		way->next->direction = get_direction(i, adjacent_pipe->variant);
		way->next->next = NULL;

		break;
	}

	int part_1_total_distance = 0;

	while (way != NULL) {
		vec_push(&coordinates, &(int[2]) {way->pipe->x, way->pipe->y});

		part_1_total_distance += 1;
		way = way->next;

		if (way == NULL) break;

		int pos[2];
		pos[0] = offsets[way->direction][0] + way->pipe->x;
		pos[1] = offsets[way->direction][1] + way->pipe->y;

		struct pipe_t *next_pipe = get_pipe_by_pos(pos[0], pos[1]);

		if (next_pipe == NULL) break;
		if (next_pipe->variant & offsets[way->direction][2]) break;

		way->next = malloc(sizeof(struct way_t));
		way->next->pipe = next_pipe;
		way->next->direction = get_direction(way->direction, next_pipe->variant);
		way->next->next = NULL;

		if (way->direction != UNKNOWN) continue;
		printf("something is wrong.\n");
		break;
	}

	double part_2_area = 0.0;
	double part_2_inner_area = 0.0;
	for (int i = 0; i < coordinates.length; i++) {
		int const *coordinate_1 = vec_get(&coordinates, i + 0);
		int const *coordinate_2 = vec_get(&coordinates, (i + 1) % coordinates.length);

		part_2_area += (
			coordinate_1[0] * coordinate_2[1] -
			coordinate_2[0] * coordinate_1[1]
		);
	}

	part_2_area = 0.5 * fabs(part_2_area);
	part_2_inner_area = part_2_area - (coordinates.length / 2.0) + 1;

	printf("part 1: %d\n", part_1_total_distance / 2);
	printf("part 2: %f\n", part_2_inner_area);

	struct way_t *tmp;
	while (way != NULL) {
		tmp = way;
		way = way->next;
		free(tmp);
	}

	vec_drop(&coordinates);
	vec_drop(&pipes);
	free(input);

	return EXIT_SUCCESS;
}

enum direction_t get_direction(enum direction_t direction, enum pipe_variant_t variant) {
	if (
		(variant & PIPE_VERTICAL && 0x1 << direction & (0x1 << UP | 0x1 << DOWN)) ||
		(variant & PIPE_HORIZONTAL && 0x1 << direction & (0x1 << LEFT | 0x1 << RIGHT))
	) {
		return direction;
	} else if (direction == UP) {
		return variant & PIPE_DOWN_RIGHT ? RIGHT : LEFT;
	} else if (direction == DOWN) {
		return variant & PIPE_UP_RIGHT ? RIGHT : LEFT;
	} else if (direction == LEFT) {
		return variant & PIPE_UP_RIGHT ? UP : DOWN;
	} else if (direction == RIGHT) {
		return variant & PIPE_UP_LEFT ? UP : DOWN;
	}

	return UNKNOWN;
}


struct pipe_t *get_pipe_by_pos(int x, int y) {
	return vec_get(&pipes, y * map_size[1] + x);
}


void get_map_size() {
	struct pipe_t last_pipe = * (struct pipe_t *) vec_get(&pipes, pipes.length - 1);
	map_size[0] = last_pipe.x + 1;
	map_size[1] = last_pipe.y + 1;
}


void expect_pipes() {
	while (cursor < input_length) {
		if (input[cursor] == '\n') {
			map_pos[0] = 0;
			map_pos[1] += 1;
			cursor += 1;
			continue;
		}

		struct pipe_t pipe;
		expect_pipe(&pipe);
		vec_push(&pipes, &pipe);
	}
}


void expect_pipe(struct pipe_t *pipe) {
	pipe->variant = PIPE_GROUND;
	pipe->x = map_pos[0];
	pipe->y = map_pos[1];

	if (input[cursor] == 'F') {
		pipe->variant = PIPE_DOWN_RIGHT;
	} else if (input[cursor] == '7') {
		pipe->variant = PIPE_DOWN_LEFT;
	} else if (input[cursor] == 'L') {
		pipe->variant = PIPE_UP_RIGHT;
	} else if (input[cursor] == 'J') {
		pipe->variant = PIPE_UP_LEFT;
	} else if (input[cursor] == '|') {
		pipe->variant = PIPE_VERTICAL;
	} else if (input[cursor] == '-') {
		pipe->variant = PIPE_HORIZONTAL;
	} else if (input[cursor] == 'S') {
		pipe->variant = PIPE_STARTING;

		memcpy(&starting_point, pipe, sizeof(struct pipe_t));
	}

	map_pos[0] += 1;
	cursor += 1;
}
