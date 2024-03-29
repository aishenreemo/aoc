#include <libcollections/vec.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"

enum expr_variant_t {
	EXPR_UNKNOWN,
	EXPR_NUMBER,
	EXPR_WORD_NUMBER,
};

struct expr_t {
	enum expr_variant_t variant;
	uint digit;
};

struct line_t {
	struct vec_t expressions;
};

void expect_line(struct line_t *line, char const *input, int *index);

int main() {
	char const *input = load_file("2023/input_01.txt");
	uint const input_len = strlen(input);

	struct vec_t lines;
	vec_init(&lines, sizeof(struct line_t));

	int index = 0;
	while (index < input_len) {
		struct line_t line;
		expect_line(&line, input, &index);
		vec_push(&lines, &line);
	}

	uint part_1_total = 0;
	uint part_2_total = 0;

	for (int i = 0; i < lines.length; i++) {
		struct line_t *line = vec_get(&lines, i);

		struct expr_t *part_1_first;
		struct expr_t *part_1_last;

		struct expr_t *part_2_first;
		struct expr_t *part_2_last;

		for (int j = 0; j < line->expressions.length; j++) {
			struct expr_t *expr = vec_get(&line->expressions, j);
			if (expr->variant != EXPR_NUMBER) continue;

			part_1_first = expr;
			break;
		}

		for (int j = line->expressions.length - 1; j >= 0; j--) {
			struct expr_t *expr = vec_get(&line->expressions, j);
			if (expr->variant != EXPR_NUMBER) continue;

			part_1_last = expr;
			break;
		}

		part_2_first = vec_get(&line->expressions, 0);
		part_2_last = vec_get(&line->expressions, line->expressions.length - 1);

		printf("part 1: %d%d ", part_1_first->digit, part_1_last->digit);
		printf("part 2: %d%d:\t", part_2_first->digit, part_2_last->digit);

		part_1_total += part_1_first->digit * 10 + part_1_last->digit;
		part_2_total += part_2_first->digit * 10 + part_2_last->digit;

		for (int j = 0; j < line->expressions.length; j++) {
			struct expr_t *expr = vec_get(&line->expressions, j);
			printf("%d ", expr->digit);
		}
		printf("%d\n", part_2_total);
	}

	printf("part 1 total: %d\n", part_1_total);
	printf("part 2 total: %d\n", part_2_total);

	for (int i = 0; i < lines.length; i++) {
		struct line_t *line = vec_get(&lines, i);
		vec_drop(&line->expressions);
	}
	vec_drop(&lines);

	return EXIT_SUCCESS;
}


void expect_line(struct line_t *line, char const *input, int *index) {
	vec_init(&line->expressions, sizeof(struct expr_t));

	while (input[*index] != '\n' && input[*index] != '\0') {
		struct expr_t *expr;
		if (isdigit(input[*index])) {
			expr->variant = EXPR_NUMBER;
			expr->digit = input[*index] - '0';
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "one", strlen("one")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 1;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "two", strlen("two")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 2;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "three", strlen("three")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 3;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "four", strlen("four")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 4;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "five", strlen("five")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 5;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "six", strlen("six")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 6;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "seven", strlen("seven")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 7;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "eight", strlen("eight")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 8;
			vec_push(&line->expressions, expr);
		} else if (strncmp(input + *index, "nine", strlen("nine")) == 0) {
			expr->variant = EXPR_WORD_NUMBER;
			expr->digit = 9;
			vec_push(&line->expressions, expr);
		}

		*index += 1;
	}

	*index += 1;
}
