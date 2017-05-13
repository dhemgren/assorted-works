#include <stdio.h>
#include <stdlib.h>
#include "konkordans.h"

static void read_string(FILE *, int, int, char **);

void read_word(FILE *index, FILE *words, int byte_pos, char **out)  {
    int w1_addr = read_int(index, byte_pos);
    int w2_addr = read_int(index, byte_pos+sizeof(int));
    int len;

    if (w1_addr == -1)  {
        printf("Error: Word index out of bounds.\n");
        exit(-1);
    }

    if (w2_addr != -1) {
        len = w2_addr - w1_addr;
    } else {
        fseek(words, 0, SEEK_END);
        len = ftell(words) - w1_addr;
    }

    read_string(words, w1_addr, len, out);
}

static void read_string(FILE *file, int byte_pos, int len, char **out) {
    *out = malloc((len + 1) * sizeof(char)); // calling function should free

    fseek(file, byte_pos, SEEK_SET);
    fread(*out, sizeof(char), len, file);
    (*out)[len] = '\0';
}

// Returns: length of int array "out"
int read_occs(FILE *index, FILE *occurrences, int byte_pos, int **out) {
    int o1_addr = read_int(index, byte_pos);
    int o2_addr = read_int(index, byte_pos+sizeof(int));
    int count;

    if (o1_addr == -1)  {
        printf("Error: Occurrence index out of bounds.\n");
        exit(-1);
    }

    if (o2_addr != -1) {
        count = (o2_addr - o1_addr) / sizeof(int);
    } else {
        fseek(occurrences, 0, SEEK_END);
        count = (ftell(occurrences) - o1_addr) / sizeof(int);
    }

    read_ints(occurrences, o1_addr, count, out);
    return count;
}

// Tar pos som en byte-position (_ej_ int-aligned)
int read_int(FILE *file, int pos) {
    int res;

    fseek(file, pos, SEEK_SET); // flytta pekare till rätt position
    fread(&res, sizeof(int), 1, file);
    
    if (feof(file)) {
        return -1;
    }

    return res;
}

int read_ints(FILE *file, int byte_pos, int count, int **out) {
    int status = fseek(file, byte_pos, SEEK_SET);
    if (status != 0) {
        printf("Error: read_ints -> fseek\n");
        exit(-1);
    }

    *out = malloc(count * sizeof(int));
    int read_amount = fread(*out, sizeof(int), count, file);
    return read_amount;
}

