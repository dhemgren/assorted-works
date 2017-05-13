#include <stdint.h>

#define HASH_TABLE_SIZE 28000
#define NL 10
#define WS 32
#define SPACE 32
#define STATIC_ARRAY_SIZE 10000
#define HASH_TABLE_SIZE 28000

// hash.c
unsigned int prefix_hash(char *);
void format_prefix(char *, uint8_t *);
void pack_prefix(uint8_t *, uint8_t *);


// read.c
void read_word(FILE *, FILE *, int, char **);
int read_occs(FILE *, FILE *, int, int **);
int read_int(FILE *, int);
int read_ints(FILE *, int, int, int **);

// constructor.c
void end_of_word(char *new_word, char *old_word);
void new_word_found(char *new_word);
void new_prefix_found(char *new_word);
void end_of_prefix(char *old_word);
void end_of_occurrence(int new_occurrence);
void write_hash_tables_to_file(void);
