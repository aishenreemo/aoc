#include <libcollections/vector.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "macros.h"


enum command_t {
	CMD_LEFT,
	CMD_RIGHT,
};


struct node_t {
	char ident[4];
	char left[4];
	char right[4];
};


static struct vector_t commands;
static struct vector_t definitions;
static struct vector_t nodes;

static char *input;
static int input_length;
static int cursor;

static struct node_t hashmap[UINT16_MAX];


uint64_t steps_to_z(struct node_t *node, bool (*is_end)(struct node_t *node));
uint64_t lcm_of_nodes_steps_to_z();

uint16_t hash(char ident[4]);
void hashmap_init();
void expect_commands();
void expect_definitions();
void expect_part_2_nodes();
void expect_char(char ch);

uint64_t gcd(uint64_t a, uint64_t b);
uint64_t lcm(struct vector_t numbers);


static bool part_1_check(struct node_t *node) {
	return strcmp(node->ident, "ZZZ") == 0;
}


static bool part_2_check(struct node_t *node) {
	return node->ident[2] == 'Z';
}


int main(void) {
	cursor = 0;
	input = load_file("2023/input_08.txt");
	input_length = strlen(input);

	vector_init(&commands, sizeof(enum command_t));
	vector_init(&definitions, sizeof(struct node_t));
	vector_init(&nodes, sizeof(struct node_t));

	expect_commands();
	expect_definitions();
	expect_part_2_nodes();
	hashmap_init();

	printf("part 1: %ld\n", steps_to_z(hashmap + hash("AAA"), part_1_check));
	printf("part 2: %ld\n", lcm_of_nodes_steps_to_z());

	vector_drop(&commands);
	vector_drop(&definitions);
	vector_drop(&nodes);

	return EXIT_SUCCESS;
}


void hashmap_init() {
	memset(hashmap, 0, sizeof(struct node_t) * UINT16_MAX);
	for (int i = 0; i < definitions.length; i++) {
		struct node_t *node = vector_get(&definitions, i);
		memcpy(hashmap + hash(node->ident), node, sizeof(struct node_t));
	}
}


uint16_t hash(char ident[4]) {
	uint16_t out = 0;

	for (int i = 0; i < 3; i++) {
		// identifiers are 3 characters from A-Z
		// A-Z holds 26 characters (5-bits can hold 32)
		// 3 * 5 bits = 15bits (uint16_t is enough to hold 15bit)
		out = out << 5;
		out += ident[i] - 'A';
	}

	return out;
}


uint64_t gcd(uint64_t a, uint64_t b) {
	return b == 0 ? a : gcd(b, a % b);
}


uint64_t lcm(struct vector_t numbers) {
	uint64_t *first = vector_get(&numbers, 0);
	uint64_t lcm_value = *first;

	for (int i = 1; i < numbers.length; i++) {
		uint64_t *number = vector_get(&numbers, i);
		lcm_value *= *number / gcd(lcm_value, *number);
	}

	return lcm_value;
}


uint64_t steps_to_z(struct node_t *node, bool (*is_end)(struct node_t *node)) {
	int index = 0;
	int steps = 0;

	struct node_t node_proxy;
	memcpy(&node_proxy, node, sizeof(struct node_t));

	while (!is_end(&node_proxy)) {
		enum command_t *command = vector_get(&commands, index);
		char *ident = *command == CMD_LEFT ? node_proxy.left : node_proxy.right;

		memcpy(&node_proxy, hashmap + hash(ident), sizeof(struct node_t));

		steps += 1;
		index += 1;
		index %= commands.length;
	}

	return steps;
}


uint64_t lcm_of_nodes_steps_to_z() {
	struct vector_t steps_arr;
	vector_init(&steps_arr, sizeof(uint64_t));

	for (int i = 0; i < nodes.length; i++) {
		struct node_t *node = vector_get(&nodes, i);
		uint64_t steps = steps_to_z(node, part_2_check);
		vector_push(&steps_arr, &steps);
	}

	uint64_t lcm_value = lcm(steps_arr);
	vector_drop(&steps_arr);

	return lcm_value;
}


void expect_part_2_nodes() {
	for (int i = 0; i < definitions.length; i++) {
		struct node_t *node = vector_get(&definitions, i);
		if (node->ident[2] != 'A') continue;
		vector_push(&nodes, node);
	}
}


void expect_commands() {
	while (input[cursor] != '\n') {
		enum command_t command;
		if (input[cursor] == 'L') {
			command = CMD_LEFT;
			vector_push(&commands, &command);
		} else if (input[cursor] == 'R') {
			command = CMD_RIGHT;
			vector_push(&commands, &command);
		}

		cursor += 1;
	}

	while (isspace(input[cursor])) {
		cursor += 1;
	}
}


void expect_definitions() {
	struct node_t node;

	while (cursor < input_length) {
		memset(&node, 0, sizeof(struct node_t));
		strncpy(node.ident, input + cursor, 3);
		node.ident[3] = '\0';
		expect_char('(');
		strncpy(node.left, input + cursor, 3);
		node.left[3] = '\0';
		expect_char(' ');
		strncpy(node.right, input + cursor, 3);
		node.right[3] = '\0';
		expect_char('\n');
		vector_push(&definitions, &node);
	}
}


void expect_char(char ch) {
	while (cursor < input_length && input[cursor - 1] != ch) {
		cursor += 1;
	}
}
