#include <libcollections/vec.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "macros.h"


enum rank_t {
	RANK_J_2,
	RANK_2,
	RANK_3,
	RANK_4,
	RANK_5,
	RANK_6,
	RANK_7,
	RANK_8,
	RANK_9,
	RANK_T,
	RANK_J,
	RANK_Q,
	RANK_K,
	RANK_A,
};

enum hand_variant_t {
	HAND_HIGH_CARD,
	HAND_ONE_PAIR,
	HAND_TWO_PAIR,
	HAND_THREE_OF_A_KIND,
	HAND_FULL_HOUSE,
	HAND_FOUR_OF_A_KIND,
	HAND_FIVE_OF_A_KIND,
};

struct hand_t {
	enum rank_t cards_1[5]; // for part 1
	enum rank_t cards_2[5]; // for part 2
	enum hand_variant_t variant_1; // for part 1
	enum hand_variant_t variant_2; // for part 2
	char str[6];
	int bid;
};


static char *input;
static int input_length;
static int cursor;

static struct vec_t hands;


void sort_hands(bool is_part_2);

bool is_lower_hand(struct hand_t *a, struct hand_t *b, bool is_part_2);

void expect_hand(struct hand_t *hand);
void expect_char(char ch);
void expect_number(int *number);


int main() {
	cursor = 0;
	input = load_file("2023/input_07.txt");
	input_length = strlen(input);

	vec_init(&hands, sizeof(struct hand_t));
	while (cursor < input_length) {
		struct hand_t hand;
		memset(&hand, 0, sizeof(struct hand_t));
		expect_hand(&hand);
		vec_push(&hands, &hand);
	}

	int part_1_total = 0;
	int part_2_total = 0;

	// sort part 1
	sort_hands(false);
	for (int i = 0; i < hands.length; i++) {
		struct hand_t *hand = vec_get(&hands, i);
		part_1_total += (i + 1) * hand->bid;
	}

	// sort part 2
	sort_hands(true);
	for (int i = 0; i < hands.length; i++) {
		struct hand_t *hand = vec_get(&hands, i);
		part_2_total += (i + 1) * hand->bid;
	}

	printf("part 1: %d\n", part_1_total);
	printf("part 2: %d\n", part_2_total);

	vec_drop(&hands);
	free(input);

	return EXIT_SUCCESS;
}


void sort_hands(bool is_part_2) {
	for (int i = 0; i < hands.length; i++) {
		struct hand_t *hand = vec_get(&hands, i);

		struct hand_t *lowest_hand = hand;
		int lowest_hand_index = i;

		for (int j = i + 1; j < hands.length; j++) {
			struct hand_t *local_hand = vec_get(&hands, j);

			if (is_lower_hand(lowest_hand, local_hand, is_part_2)) continue;

			lowest_hand = local_hand;
			lowest_hand_index = j;
		}

		hands.items[lowest_hand_index] = hand;
		hands.items[i] = lowest_hand;
	}
}


bool is_lower_hand(struct hand_t *a, struct hand_t *b, bool is_part_2) {
	enum hand_variant_t a_variant = is_part_2 ? a->variant_2 : a->variant_1;
	enum hand_variant_t b_variant = is_part_2 ? b->variant_2 : b->variant_1;

	if (a_variant == b_variant) {
		for (int i = 0; i < 5; i++) {
			enum rank_t a_rank = is_part_2 ? a->cards_2[i] : a->cards_1[i];
			enum rank_t b_rank = is_part_2 ? b->cards_2[i] : b->cards_1[i];

			if (a_rank == b_rank) continue;

			return a_rank < b_rank;
		}
	}

	return a_variant < b_variant;
}


enum rank_t get_rank(char ch, bool is_part_2) {
	if (ch == 'J') {
		return is_part_2 ? RANK_J_2 : RANK_J;
	} else if (ch == 'A') {
		return RANK_A;
	} else if (ch == 'K') {
		return RANK_K;
	} else if (ch == 'Q') {
		return RANK_Q;
	} else if (ch == 'T') {
		return RANK_T;
	} else if (ch == '9') {
		return RANK_9;
	} else if (ch == '8') {
		return RANK_8;
	} else if (ch == '7') {
		return RANK_7;
	} else if (ch == '6') {
		return RANK_6;
	} else if (ch == '5') {
		return RANK_5;
	} else if (ch == '4') {
		return RANK_4;
	} else if (ch == '3') {
		return RANK_3;
	} else {
		return RANK_2;
	}
}


enum hand_variant_t get_variant(enum rank_t cards[5]) {
	int uniq[RANK_A + 1];
	int uniq_length = 0;
	int uniq_set[5];

	memset(uniq, 0, sizeof(int) * (RANK_A + 1));
	memset(uniq_set, 0, sizeof(int) * 5);

	for (int i = 0; i < 5; i++) {
		uniq[cards[i]] += 1;
	}

	for (int i = RANK_2; i < RANK_A + 1; i++) {
		if (uniq[i] == 0) continue;

		uniq_set[uniq_length] = uniq[i] + uniq[RANK_J_2];
		uniq_length += 1;
	}

	if (uniq_length == 1) {
		return HAND_FIVE_OF_A_KIND;
	} else if (uniq_length == 0 && uniq[RANK_J_2] != 0) {
		return HAND_FIVE_OF_A_KIND;
	} else if (uniq_length == 2 && (uniq_set[0] == 4 || uniq_set[1] == 4)) {
		return HAND_FOUR_OF_A_KIND;
	} else if (uniq_length == 2 && (uniq_set[0] == 3 || uniq_set[1] == 3)) {
		return HAND_FULL_HOUSE;
	} else if (uniq_length == 3 && (
		uniq_set[0] == 3 ||
		uniq_set[1] == 3 ||
		uniq_set[2] == 3
	)) {
		return HAND_THREE_OF_A_KIND;
	} else if (uniq_length == 3 && (
		uniq_set[0] == 2 ||
		uniq_set[1] == 2 ||
		uniq_set[2] == 2
	)) {
		return HAND_TWO_PAIR;
	} else if (uniq_length == 4) {
		return HAND_ONE_PAIR;
	} else {
		return HAND_HIGH_CARD;
	}
}


void expect_hand(struct hand_t *hand) {
	for (int i = 0; i < 5; i++) {
		hand->cards_1[i] = get_rank(input[cursor + i], false);
		hand->cards_2[i] = get_rank(input[cursor + i], true);
	}

	hand->variant_1 = get_variant(hand->cards_1);
	hand->variant_2 = get_variant(hand->cards_2);

	strncpy(hand->str, input + cursor, 5);

	expect_char(' ');
	expect_number(&hand->bid);
	expect_char('\n');
}


void expect_char(char ch) {
	while (cursor < input_length && input[cursor - 1] != ch) {
		cursor += 1;
	}
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
