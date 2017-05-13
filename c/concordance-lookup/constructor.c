#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "konkordans.h"

FILE *words;
FILE *occurrences;
FILE *word_index;
FILE *occurrence_index;

int hash_table_prefix_start[HASH_TABLE_SIZE];
int hash_table_prefix_end[HASH_TABLE_SIZE];

static void close_files(void);

int main(int argc, char * argv[]){
    word_index = fopen("/var/tmp/ewdh-word_index.bin", "w");
    words = fopen("/var/tmp/ewdh-words.txt", "w");
    occurrence_index = fopen("/var/tmp/ewdh-occurrence_index.bin", "w");
    occurrences = fopen("/var/tmp/ewdh-occurrences.bin", "w");

    char char_buffer[STATIC_ARRAY_SIZE];
    int char_buffer_len = 0; 

    char current_word[STATIC_ARRAY_SIZE];

    // Fyll hashtabellerna med -1 så vi vet sen när vi söker om något prefix saknas.
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hash_table_prefix_start[i] = -1;
        hash_table_prefix_end[i] = -1;
    }

    int c;
    while (1) {
        switch (c = getchar()) {
            case EOF:
                end_of_prefix(current_word);
                write_hash_tables_to_file();
                close_files();
                exit(0);

            case SPACE: // Ord klart.

                // Lägg på en null byte som strängterminering på char_buffer.
                char_buffer[char_buffer_len] = '\0';
                // Bara för korrekthet (att char_buffer_len *alltid* ska vara
                // längden på char_buffer) inkrementeras även char_buffer_len
                // här.
                char_buffer_len++;

                end_of_word(char_buffer, current_word);

                // Uppdatera current_word.
                strcpy(current_word, char_buffer);

                // Sätt char_buffer_len till 0 för att "rensa" char_buffer.
                char_buffer_len = 0;
                break;

            case NL: // Tal (occurrence) klart.

                // Här är det OK att använda atoi (ASCII to int) för att ASCII
                // och ISO-Latin-1 har samma koder för siffrorna 0-9.
                char_buffer[char_buffer_len]='\0';
                end_of_occurrence(atoi(char_buffer));

                // Sätt char_buffer_len till 0 för att "rensa" char_buffer.
                char_buffer_len = 0;
                break;

            default: // Lägg på en char på char_buffer
                char_buffer[char_buffer_len] = c;
                char_buffer_len++;
        }
    }
}

// Här kollar vi om det ordet vi just läst in är nytt, och i så fall även om det
// har ett nytt prefix.
void end_of_word(char *new_word, char *old_word) {

    if (strcmp(new_word, old_word) != 0) {
        // Nytt ord funnet. Kolla först efter nytt prefix (så vi kan köra
        // end_of_prefix och new_prefix_found innan vi har skrivit
        // det nya ordet och ändrat skrivpositionen i word_index och
        // occurrence_index).
        if (prefix_hash(new_word) != prefix_hash(old_word)) {
            // Nytt prefix funnet.
            end_of_prefix(old_word);
            new_prefix_found(new_word);
        }

        // Skriv pekare till det nya ordets position.
        new_word_found(new_word);
    }
}

void new_word_found(char *new_word) {

    // Skriv pekare mot det nya ordets position i words.txt till word_index.bin.
    int new_word_address = (int) ftell(words);
    fwrite(&new_word_address, sizeof(int), 1, word_index);

    // Skriv det nya ordet till words.txt.
    fwrite(new_word, strlen(new_word), 1, words);

    // Skriv pekare mot det nya ordets första position i occurrences.bin till
    // occurrence_index.bin.
    int new_occurrence_address = (int) ftell(occurrences);
    fwrite(&new_occurrence_address, sizeof(int), 1, occurrence_index);
}

// Skriv pekare mot det nya prefixets position i word_index.bin OCH
// occurrence_index.bin till hash_table_prefix_start[prefix_hash(new_word)].
void new_prefix_found(char *new_word) {

    // Försäkra att nuvarande position är densamma i word_index och
    // occurrence_index.
    if (ftell(word_index) != ftell(occurrence_index)) {
        printf("Error: writing position in word_index.bin and occurrence_index.bin"
                "misaligned.\n");
    }

    // Addressen vi ska skriva till hashtabellen blir ftell(word_index) eftersom
    // vi ännu inte har skrivit en pekare för det nya ordet till word_index.
    // Skrivpositionen i word_index är alltså nu precis där det nya ordet skall
    // börja.
    hash_table_prefix_start[prefix_hash(new_word)] = ftell(word_index);
}

void end_of_prefix(char *old_word) {

    // Försäkra att nuvarande position är densamma i word_index och
    // occurrence_index.
    if (ftell(word_index) != ftell(occurrence_index)) {
        printf("Error: writing position in word_index.bin and occurrence_index.bin"
                "misaligned.\n");
    }

    // Eftersom vi redan har skrivit old_word till word_index blir addressen
    // till old_words position ftell(word_index) - sizeof(int).
    hash_table_prefix_end[prefix_hash(old_word)] = ftell(word_index) - sizeof(int);
}

// Skriv talet new_occurrence till occurrences.bin.
void end_of_occurrence(int new_occurrence) {
    fwrite(&new_occurrence, sizeof(int), 1, occurrences);
}

// Skriv hela hashtabellen till fil
void write_hash_tables_to_file(void) {
    FILE *hash_file_start = fopen("/var/tmp/ewdh-hash_start.bin", "w");
    fwrite(hash_table_prefix_start, sizeof(hash_table_prefix_start), 1, hash_file_start);
    fclose(hash_file_start);

    FILE *hash_file_end = fopen("/var/tmp/ewdh-hash_end.bin", "w");
    fwrite(hash_table_prefix_end, sizeof(hash_table_prefix_end), 1, hash_file_end);
    fclose(hash_file_end);
}

static void close_files(void){
    fclose(words);
    fclose(word_index);
    fclose(occurrences);
    fclose(occurrence_index);
}
