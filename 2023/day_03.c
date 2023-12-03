#include <libcollections/vector.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"

enum expr_variant_t {
	EXPR_UNKNOWN,
	EXPR_PERIOD,
	EXPR_SYMBOL,
	EXPR_NUMBER,
	EXPR_NEWLINE,
};

struct expr_period_t {
	enum expr_variant_t variant;
	int x;
	int y;
	int width;
};

struct expr_number_t {
	enum expr_variant_t variant;
	int x;
	int y;
	int width;
	int value;
};

struct expr_symbol_t {
	enum expr_variant_t variant;
	int x;
	int y;
	char value;
};

struct expr_newline_t {
	enum expr_variant_t variant;
	int x;
	int y;
};

union expr_t {
	enum expr_variant_t variant;
	struct expr_period_t period;
	struct expr_number_t number;
	struct expr_symbol_t symbol;
	struct expr_newline_t newline;
};

struct position_t {
	int index;
	int x;
	int y;
};


void expect_expr(
	union expr_t *const expr,
	struct position_t *const position,
	char const *const input
);

void expect_number(
	struct expr_number_t *const number_expr,
	struct position_t *const position,
	char const *const input
);

void expect_period(
	struct expr_period_t *const period_expr,
	struct position_t *const position,
	char const *const input
);

bool is_part_number(union expr_t *expr);
bool get_gear_part_numbers(union expr_t *sym_expr, union expr_t part_gears[2]);

struct position_t position;
struct vector_t expressions;


int main(void) {
	char *const input = load_file("2023/input_03.txt");
	uint const input_len = strlen(input);

	memset(&position, 0, sizeof(struct position_t));
	vector_init(&expressions, sizeof(union expr_t));

	while (position.index < input_len) {
		union expr_t expr;
		expect_expr(&expr, &position, input);
		vector_push(&expressions, &expr);
	}

	int part_1_total = 0;
	for (int i = 0; i < expressions.length; i++) {
		union expr_t *expr = vector_get(&expressions, i);
		if (!is_part_number(expr)) continue;
		part_1_total += expr->number.value;
	}

	int part_2_total = 0;
	for (int i = 0; i < expressions.length; i++) {
		union expr_t *expr = vector_get(&expressions, i);
		union expr_t part_gears[2];
		memset(part_gears, 0, sizeof(union expr_t) * 2);

		if (!get_gear_part_numbers(expr, part_gears)) continue;
		part_2_total += part_gears[0].number.value * part_gears[1].number.value;
	}

	printf("part 1 total: %d\n", part_1_total);
	printf("part 2 total: %d\n", part_2_total);

	vector_drop(&expressions);
	free(input);
}


bool get_gear_part_numbers(union expr_t *sym_expr, union expr_t part_gears[2]) {
	if (sym_expr->variant != EXPR_SYMBOL) return false;
	if (sym_expr->symbol.value != '*') return false;

	int part_gear_count = 0;
	for (int i = 0; i < expressions.length; i++) {
		union expr_t *expr = vector_get(&expressions, i);

		if (expr->variant != EXPR_NUMBER) continue;
		if (sym_expr->symbol.y > expr->number.y + 1) continue;
		if (sym_expr->symbol.y < expr->number.y - 1) continue;
		if (sym_expr->symbol.x < expr->number.x - 1) continue;
		if (sym_expr->symbol.x > expr->number.x + expr->number.width) continue;

		if (part_gear_count >= 2) {
			memset(part_gears, 0, sizeof(union expr_t) * 2);
			return false;
		};

		memcpy(part_gears + part_gear_count, expr, sizeof(union expr_t));
		part_gear_count += 1;
	}

	if (part_gear_count < 2) {
		memset(part_gears, 0, sizeof(union expr_t) * 2);
		return false;
	};

	return true;
}


bool is_part_number(union expr_t *num_expr) {
	if (num_expr->variant != EXPR_NUMBER) return false;

	for (int i = 0; i < expressions.length; i++) {
		union expr_t *expr = vector_get(&expressions, i);

		if (expr->symbol.y > num_expr->number.y + 1) break;
		if (expr->variant != EXPR_SYMBOL) continue;
		if (expr->symbol.y < num_expr->number.y - 1) continue;
		if (expr->symbol.x < num_expr->number.x - 1) continue;
		if (expr->symbol.x > num_expr->number.x + num_expr->number.width) continue;

		return true;
	}

	return false;
}


void expect_expr(
	union expr_t *const expr,
	struct position_t *const position,
	char const *const input
) {
	while (input[position->index] != '\0') {
		if (isdigit(input[position->index])) {
			expect_number(&expr->number, position, input);
			expr->variant = EXPR_NUMBER;
			break;
		} else if (input[position->index] == '.') {
			expect_period(&expr->period, position, input);
			expr->variant = EXPR_PERIOD;
			break;
		} else if (input[position->index] == '\n') {
			expr->variant = EXPR_NEWLINE;
			expr->newline.x = position->x;
			expr->newline.y = position->y;
			position->index += 1;
			position->y += 1;
			position->x = 0;
			break;
		} else {
			expr->variant = EXPR_SYMBOL;
			expr->symbol.x = position->x;
			expr->symbol.y = position->y;
			expr->symbol.value = input[position->index];
			position->index += 1;
			position->x += 1;
			break;
		}
	}
}


void expect_number(
	struct expr_number_t *const number_expr,
	struct position_t *const position,
	char const *const input
) {
	int start_index = position->index;
	number_expr->x = position->x;
	number_expr->y = position->y;
	number_expr->width = 0;

	while (isdigit(input[position->index])) {
		number_expr->width += 1;
		position->index += 1;
		position->x += 1;
	}

	char substring[number_expr->width + 1];
	strncpy(substring, input + start_index, number_expr->width);
	substring[number_expr->width] = '\0';

	number_expr->value = atoi(substring);
}


void expect_period(
	struct expr_period_t *const period_expr,
	struct position_t *const position,
	char const *const input
) {
	period_expr->x = position->x;
	period_expr->y = position->y;
	period_expr->width = 0;

	while (input[position->index] == '.') {
		period_expr->width += 1;
		position->index += 1;
		position->x += 1;
	}
}
