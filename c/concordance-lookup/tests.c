#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "konkordans.h"
#define NUM_WORDS 2

static void test_words_and_occs(int);
static void test_hash_function(char *);
static void test_konkordans(char *);
static void find_largest_prefix_group(void);

int midpoint(int l, int r) {
    int a = l + r;
    int b = a / 4;
    int c = b / 2;
    int d = c * 4;
}

static void test_words_and_occs(int byte_pos) {
    FILE *words = fopen("/var/tmp/ewdh-words.txt", "r");
    FILE *word_index = fopen("/var/tmp/ewdh-word_index.bin", "r");

    char *str;
    read_word(word_index, words, byte_pos, &str);
    printf("Word at byte position %d: %s\n", byte_pos, str);

    FILE *occs = fopen("/var/tmp/ewdh-occurrences.bin", "r");
    FILE *occ_index = fopen("/var/tmp/ewdh-occurrence_index.bin", "r");

    int *out;
    int count = read_occs(occ_index, occs, byte_pos, &out);
    for(int i = 0; i < count; i++){
        printf("Occurrence %d: %d\n", i, out[i]);
    }
}

static void test_hash_function(char *input_str) {
    printf("---- Hashing \"%s\" ----\n", input_str);

    uint8_t formatted_prefix[3];
    format_prefix(input_str, formatted_prefix);
    printf("Formatted prefix = %u, %u, %u\n",
            formatted_prefix[0],
            formatted_prefix[1],
            formatted_prefix[2]);

    uint8_t packed_prefix[3];
    pack_prefix(formatted_prefix, packed_prefix);
    printf("Packed prefix = %u, %u, %u\n",
            packed_prefix[0],
            packed_prefix[1],
            packed_prefix[2]);

    unsigned int hash = prefix_hash(input_str);
    printf("Hash value = %u\n", hash);
}

static void test_konkordans(char *in) {
    FILE *words;
    FILE *occurrences;
    FILE *word_index;
    FILE *occurrence_index;

    words = fopen("/var/tmp/ewdh-words.txt", "r");
    occurrences = fopen("/var/tmp/ewdh-occurrences.bin", "r");
    word_index = fopen("/var/tmp/ewdh-word_index.bin", "r");
    occurrence_index = fopen("/var/tmp/ewdh-occurrence_index.bin", "r");

    FILE *start_hash_table_file = fopen("/var/tmp/ewdh-hash_start.bin", "r");
    int start_hash_table[HASH_TABLE_SIZE];
    fread(start_hash_table, HASH_TABLE_SIZE * sizeof(int), 1, start_hash_table_file);

    int start_hash_value = prefix_hash(in);
    printf("start_hash_value: %d\n", start_hash_value);

    int start_word_pos = start_hash_table[start_hash_value];
    printf("start_word_pos: %d\n", start_word_pos);

    char* start_fetched_word;
    read_word(word_index, words, start_word_pos, &start_fetched_word);
    printf("start_word: %s\n", start_fetched_word);


    FILE *end_hash_table_file = fopen("/var/tmp/ewdh-hash_end.bin", "r");
    int end_hash_table[HASH_TABLE_SIZE];
    fread(end_hash_table, HASH_TABLE_SIZE * sizeof(int), 1, end_hash_table_file);

    int end_hash_value = prefix_hash(in);
    printf("end_hash_value: %d\n", end_hash_value);

    int end_word_pos = end_hash_table[end_hash_value];
    printf("end_word_pos: %d\n", end_word_pos);

    char* end_fetched_word;
    read_word(word_index, words, end_word_pos, &end_fetched_word);
    printf("end_word: %s\n", end_fetched_word);
}

static void find_largest_prefix_group(void) {
    FILE *start_hash_table_file = fopen("/var/tmp/ewdh-hash_start.bin", "r");
    int start_hash_table[HASH_TABLE_SIZE];
    fread(start_hash_table, HASH_TABLE_SIZE * sizeof(int), 1, start_hash_table_file);

    FILE *end_hash_table_file = fopen("/var/tmp/ewdh-hash_end.bin", "r");
    int end_hash_table[HASH_TABLE_SIZE];
    fread(end_hash_table, HASH_TABLE_SIZE * sizeof(int), 1, end_hash_table_file);

    int max = 0;
    int pos = -1;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (max < (end_hash_table[i] - start_hash_table[i])) {
            max = end_hash_table[i] - start_hash_table[i];
            pos = start_hash_table[i];
        }
    }

    FILE *words = fopen("/var/tmp/ewdh-words.txt", "r");
    FILE *word_index = fopen("/var/tmp/ewdh-word_index.bin", "r");

    printf("Max prefix start position: %d\n", pos);
    printf("Antal ord: %d\n", max / 4);

    char* start_fetched_word;
    read_word(word_index, words, pos, &start_fetched_word);
    printf("Max prefix start word: %s\n", start_fetched_word);
    read_word(word_index, words, pos+4, &start_fetched_word);
    printf("Max prefix second word: %s\n", start_fetched_word);
}
