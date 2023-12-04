#include <libcollections/vector.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"


struct line_t {
	int card_id;
	int count;
	struct vector_t winning_nums;
	struct vector_t nums;
};


void expect_line(struct line_t *line, char const *input, int *const index);
void expect_number(int *number, char const* input, int *const index);

int count_matches(struct line_t *line);
int count_scratch_cards(struct vector_t *lines, struct line_t *line);

int main() {
	char *input = load_file("2023/input_04.txt");
	uint input_len = strlen(input);

	struct vector_t lines;
	vector_init(&lines, sizeof(struct line_t));

	int index = 0;
	while (index < input_len) {
		struct line_t line;
		vector_init(&line.winning_nums, sizeof(int));
		vector_init(&line.nums, sizeof(int));
		expect_line(&line, input, &index);
		vector_push(&lines, &line);
	}

	for (int i = 0; i < lines.length; i++) {
		struct line_t *line = vector_get(&lines, i);
		printf("Card %d: ", line->card_id);

		for (int j = 0; j < line->winning_nums.length; j++) {
			int *winning_num = vector_get(&line->winning_nums, j);
			printf("%d ", *winning_num);
		}

		printf("| ");

		for (int j = 0; j < line->nums.length; j++) {
			int *num = vector_get(&line->nums, j);
			printf("%d ", *num);
		}

		printf("\n");
	}

	int part_1_total = 0;
	int part_2_total = 0;
	for (int i = 0; i < lines.length; i++) {
		struct line_t *line = vector_get(&lines, i);

		part_2_total += line->count;

		int count = count_matches(line);
		if (count == 0) continue;

		int part_1_tmp = 1;
		for (int j = 1; j < count; j++) {
			part_1_tmp *= 2;
		}

		part_1_total += part_1_tmp;

		int part_2_tmp = count + i + 1;
		int part_2_max = part_2_tmp > lines.length ? lines.length : part_2_tmp;

		for (int j = i + 1; j < part_2_max; j++) {
			struct line_t *line_next = vector_get(&lines, j);
			line_next->count += line->count;
		}

	}

	printf("part 1 total: %d\n", part_1_total);
	printf("part 2 total: %d\n", part_2_total);

	for (int i = 0; i < lines.length; i++) {
		struct line_t *line = vector_get(&lines, i);
		vector_drop(&line->winning_nums);
		vector_drop(&line->nums);
	}

	vector_drop(&lines);
}

int count_matches(struct line_t *line) {
	int count = 0;
	for (int i = 0; i < line->nums.length; i++) {
		int *num = vector_get(&line->nums, i);

		for (int j = 0; j < line->winning_nums.length; j++) {
			int *winning_num = vector_get(&line->winning_nums, j);
			if (*num != *winning_num) continue;

			count += 1;
			break;
		}
	}

	return count;
}


void expect_line(struct line_t *line, char const *input, int *const index) {
	line->count = 1;

	while (input[*index] != ':') {
		if (isdigit(input[*index])) {
			expect_number(&line->card_id, input, index);
			break;
		}

		*index += 1;
	}

	*index += 1;

	while (input[*index] != '|') {
		if (isdigit(input[*index])) {
			int number;
			expect_number(&number, input, index);
			vector_push(&line->winning_nums, &number);
		} else {
			*index += 1;
		}
	}

	while (input[*index] != '\n' && input[*index] != '\0') {
		if (isdigit(input[*index])) {
			int number;
			expect_number(&number, input, index);
			vector_push(&line->nums, &number);
		} else {
			*index += 1;
		}
	}

	*index += 1;
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
