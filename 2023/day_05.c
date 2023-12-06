#include <libcollections/vector.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "macros.h"


enum conversion_variant_t {
	CVN_SEED,
	CVN_SOIL,
	CVN_FERTILIZER,
	CVN_WATER,
	CVN_LIGHT,
	CVN_TEMPERATURE,
	CVN_HUMIDITY,
	CVN_LOCATION,
};


struct map_t {
	uint64_t destination;
	uint64_t source;
	uint64_t range;
};


struct conversion_t {
	enum conversion_variant_t from;
	enum conversion_variant_t to;
	struct vector_t maps;
};

struct seed_t {
	uint64_t start;
	uint64_t range;
};


static char *input;
static int input_length;
static int cursor;

static struct vector_t seeds;
static struct vector_t seeds_2;
static struct vector_t conversions;


uint64_t get_mapped_value(uint64_t seed);

void expect_seeds();
void expect_seeds_2();
void expect_conversions();
void expect_number(uint64_t *seed);
void expect_unit(enum conversion_variant_t *conv_variant);
void expect_maps(struct vector_t *maps);
void expect_char(char ch);


int main(void) {
	cursor = 0;
	input = load_file("2023/input_05.txt");
	input_length = strlen(input);

	vector_init(&seeds, sizeof(uint64_t));
	vector_init(&seeds_2, sizeof(struct seed_t));
	vector_init(&conversions, sizeof(struct conversion_t));

	expect_seeds();
	expect_conversions();

	for (int i = 0; i < seeds.length; i += 2) {
		struct seed_t seed;
		uint64_t *start = vector_get(&seeds, i + 0);
		uint64_t *range = vector_get(&seeds, i + 1);
		seed.start = *start;
		seed.range = *range;
		vector_push(&seeds_2, &seed);
	}

	uint64_t part_1_lowest = UINT64_MAX;
	uint64_t part_2_lowest = UINT64_MAX;
	for (int i = 0; i < seeds.length; i++) {
		uint64_t *seed = vector_get(&seeds, i);
		uint64_t mapped_value = get_mapped_value(*seed);

		if (mapped_value > part_1_lowest) continue;
		part_1_lowest = mapped_value;
	}

	for (int i = 0; i < seeds_2.length; i++) {
		printf("processing seed %d....\n", i);
		struct seed_t *seed = vector_get(&seeds_2, i);

		for (int j = 0; j < seed->range; j++) {
			uint64_t seed_value = seed->start + j;
			uint64_t mapped_value = get_mapped_value(seed_value);

			if (mapped_value > part_2_lowest) continue;
			part_2_lowest = mapped_value;
		}
	}

	printf("part 1: %ld\n", part_1_lowest);
	printf("part 2: %ld\n", part_2_lowest);

	for (int i = 0; i < conversions.length; i++) {
		struct conversion_t *conv = vector_get(&conversions, i);
		vector_drop(&conv->maps);
	}

	vector_drop(&seeds);
	vector_drop(&seeds_2);
	vector_drop(&conversions);

	free(input);
	return EXIT_SUCCESS;
}


uint64_t get_mapped_value(uint64_t seed) {
	uint64_t mapped_value = seed;

	for (int i = 0; i < conversions.length; i++) {
		struct conversion_t *conv = vector_get(&conversions, i);

		for (int j = 0; j < conv->maps.length; j++) {
			struct map_t *map = vector_get(&conv->maps, j);

			if (mapped_value < map->source) continue;
			if (mapped_value > map->source + map->range) continue;

			mapped_value = map->destination + (mapped_value - map->source);
			break;
 		}
	}

	return mapped_value;
}


void expect_seeds() {
	while (input[cursor] != '\n') {
		if (isdigit(input[cursor])) {
			uint64_t seed;
			expect_number(&seed);
			vector_push(&seeds, &seed);
		}

		cursor += 1;
	}

	while (isspace(input[cursor])) {
		cursor += 1;
	}
}


void expect_conversions() {
	while (input[cursor] != '\0') {
		struct conversion_t conv;
		vector_init(&conv.maps, sizeof(struct map_t));

		expect_unit(&conv.from);
		expect_char('-');
		expect_char('t');
		expect_char('-');
		expect_unit(&conv.to);
		expect_char('\n');
		expect_maps(&conv.maps);

		vector_push(&conversions, &conv);
	}
}


void expect_number(uint64_t *number) {
	int start = cursor;
	while (isdigit(input[cursor])) {
		cursor += 1;
	}

	int length = cursor - start;
	char substring[length + 1];

	strncpy(substring, input + start, length);
	substring[length] = '\0';

	*number = strtoul(substring, NULL, 10);
}


void expect_unit(enum conversion_variant_t *conv_variant) {
	if (strncmp("seed", input + cursor, strlen("seed")) == 0) {
		*conv_variant = CVN_SEED;
		cursor += strlen("seed");
	} else if (strncmp("soil", input + cursor, strlen("soil")) == 0) {
		*conv_variant = CVN_SOIL;
		cursor += strlen("soil");
	} else if (strncmp("fertilizer", input + cursor, strlen("fertilizer")) == 0) {
		*conv_variant = CVN_FERTILIZER;
		cursor += strlen("fertilizer");
	} else if (strncmp("water", input + cursor, strlen("water")) == 0) {
		*conv_variant = CVN_WATER;
		cursor += strlen("water");
	} else if (strncmp("light", input + cursor, strlen("light")) == 0) {
		*conv_variant = CVN_LIGHT;
		cursor += strlen("light");
	} else if (strncmp("temperature", input + cursor, strlen("temperature")) == 0) {
		*conv_variant = CVN_TEMPERATURE;
		cursor += strlen("temperature");
	} else if (strncmp("humidity", input + cursor, strlen("humidity")) == 0) {
		*conv_variant = CVN_HUMIDITY;
		cursor += strlen("humidity");
	} else if (strncmp("location", input + cursor, strlen("location")) == 0) {
		*conv_variant = CVN_LOCATION;
		cursor += strlen("location");
	}
}


void expect_maps(struct vector_t *maps) {
	while (input[cursor] != '\0') {
		if (strncmp("\n\n", input + cursor, 2) == 0) {
			cursor += 2;
			break;
		}

		if (!isdigit(input[cursor])) {
			cursor += 1;
			continue;
		}

		struct map_t map;
		expect_number(&map.destination);
		expect_char(' ');
		expect_number(&map.source);
		expect_char(' ');
		expect_number(&map.range);
		vector_push(maps, &map);
	}
}


void expect_char(char ch) {
	while (cursor < input_length && input[cursor - 1] != ch) {
		cursor += 1;
	}
}
