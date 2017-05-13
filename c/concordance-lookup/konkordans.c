#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "konkordans.h"

FILE *words;
FILE *word_index;
FILE *occurrences;
FILE *occurrence_index;
FILE *hash_file_start;
FILE *hash_file_end; 
FILE *corpus;

int prefix_start[HASH_TABLE_SIZE];
int prefix_end[HASH_TABLE_SIZE];

static void confirm_printout(int, char *);
static int linear_search(char *);
static int binary_search(char *);
static int midpoint(int, int);
static void print_occurrences(FILE*, FILE*, FILE*, int);

int main(int argc, char *argv[]) {

    if(argc < 3){
        printf("\n Ange parametrar [ord] [korpus] \n");
    }

    // Öppna alla filer som ska läsas.
    words = fopen("bin/words.txt", "r");
    word_index = fopen("bin/word_index.bin", "r");
    occurrences = fopen("bin/occurrences.bin", "r");
    occurrence_index = fopen("bin/occurrence_index.bin", "r");

    hash_file_start = fopen("bin/hash_start.bin", "r");
    hash_file_end = fopen("bin/hash_end.bin", "r");

    corpus = fopen(argv[2], "r");

    // Läs in hela hashtabellen i internminnet.
    fread(prefix_start, HASH_TABLE_SIZE * sizeof(int), 1, hash_file_start);
    fread(prefix_end, HASH_TABLE_SIZE * sizeof(int), 1, hash_file_end);

    // Binärsök efter ordet i argv[1]
    int word_addr = binary_search(argv[1]);

    // Ord inte hittat
    if(word_addr == -1) {
        printf("Inga resultat funna för ordet \"%s\".\n", argv[1]);
        exit(-1);
    }

    // Skriv ut funna förekomster i Korpus på stdout
    print_occurrences(corpus, occurrence_index, occurrences, word_addr);

    // Stäng alla filer.
    fclose(corpus);
    fclose(words);
    fclose(word_index);
    fclose(occurrences);
    fclose(occurrence_index);
    fclose(hash_file_start);
    fclose(hash_file_end);
    return 0;
}

// Returnerar -1 om ordet inte finns, 0 om det finns. Kanske inte kommer
// behövas, men skadar inte.
static int linear_search(char *search_word) {

    // Kolla om det finns något ord med samma prefix.
    if (prefix_start[prefix_hash(search_word)] == -1) {
        // Det finns inga ord med samma prefix som sökordet.
        printf("Det finns inga ord med samma prefix som sökordet.\n");
        return -1;
    }

    // Gå framåt och kolla varje ord. Stoppvillkoren är:
    // 1. Vi har nått slutet av hashtabellen. // TODO: bugg i det här
    // slutvillkoret
    // 2. Vi har funnit sökordet.
    // 3. Vi har kommit in på ord med fel prefix.
    char *current_word;
    for (int i = 0; prefix_hash(search_word) + i * sizeof(int) < HASH_TABLE_SIZE; i++) {

        // Läs nästa ord till current_word.
        read_word(word_index, words, prefix_start[prefix_hash(search_word)] +
                (i * sizeof(int)), &current_word);

        // Kolla om current_word är sökordet.
        if (strcmp(current_word, search_word) == 0) {
            printf("Sökord funnet på address %lu.\n", prefix_start[prefix_hash(search_word)] +
                    (i * sizeof(int)));
            return 0;
        }

        // Kolla om vi har kommit in på ord med fel prefix
        if (prefix_hash(current_word) != prefix_hash(search_word)) {
            printf("Sökordet finns inte i konkordansen.\n");
            return -1;
        }
    }
    printf("Sökordet finns inte i konkordansen.\n");
    return -1;
}

// Returnerar addressen till sökordet om funnet, annars -1.
static int binary_search(char *search_word) {
    // Kolla om det finns något ord med samma prefix.
    if (prefix_start[prefix_hash(search_word)] == -1)
        return -1;

    int l_address = prefix_start[prefix_hash(search_word)];
    int r_address = prefix_end[prefix_hash(search_word)];

    int mid_address;
    char *mid_word;
    int cmp;
    
    while (r_address - l_address > 4) {
        // Ta ut mittpunkten
        int mid_address = midpoint(l_address, r_address);
        read_word(word_index, words, mid_address, &mid_word);

        //printf("mid_word: %s\n", mid_word);
        cmp = strcmp(search_word, mid_word);

        if (cmp > 0) { // search_word > mid_word
            //printf("search_word > mid_word\n");
            l_address = mid_address;
        }
        else if (cmp == 0) { // search_word == mid_word
            //printf("search_word == mid_word\n");
            // Sökordet funnet.
            return mid_address;
        }
        else if (cmp < 0) { // search_word < mid_word
            //printf("search_word < mid_word\n");
            r_address = mid_address;
        }
    }

    // Nu finns det bara två tal kvar, vilka pekas på av l_addr och r_addr.
    // Kolla båda dem.

    char *l_word;
    char *r_word;

    read_word(word_index, words, l_address, &l_word);
    read_word(word_index, words, r_address, &r_word);

    // Kolla om någon av start och slutpunkterna är rätt ord
    if (strcmp(search_word, l_word) == 0)
        return l_address;
    if (strcmp(search_word, r_word) == 0)
        return r_address;

    // Ordet fanns inte.
    return -1;
}

// Returnerar floor av l + r / 2, aligned till multiplar av 4.
static int midpoint(int l, int r) {
    int a = l + r;
    int b = a / 4;
    int c = b / 2;
    int d = c * 4;
}

// Skriver ut raderna i corpus som occurrences pekar på via occ_index[word_addr]
static void print_occurrences(FILE *corpus, FILE* occ_index, FILE* occurrences, int word_addr){


    int occ_start = read_int(occ_index, word_addr);
    int occ_end = read_int(occ_index, word_addr+sizeof(int));
    int num_occs;

    // Kolla antal occurrences. Om det inte finns någon efterföljande
    // occurrence-grupp, kommer occ_end att vara -1.
    if (occ_end == -1) {
        fseek(occurrences, 0, SEEK_END);
        num_occs = (ftell(occurrences) - occ_start) / sizeof(int);
    }
    else {
        num_occs = (occ_end - occ_start) / sizeof(int);
    }

    char *searchword;
    read_word(word_index, words, word_addr, &searchword);
    
    int occ_address;
    for (int i = 0; i < num_occs; i++) {
 
        // Ta ut nästa adress
        occ_address = read_int(occurrences, occ_start + (sizeof(int) * i));
        
        // Clampa adressen nedåt till 0
        int result_string_start = occ_address - 30;
        if (result_string_start < 0)
            result_string_start = 0;
       
        // Läs ut strängarna
        char str[61];
        int c_i = 0;
        fseek(corpus, result_string_start, SEEK_SET);
        int c = getc(corpus);
        while(!feof(corpus)){
            // Avbryt om mer än 60 tecken hämtade
            if(c_i > 59)
                break;

            // Byt ut alla radbrytningar mot blanksteg
            if(c == NL){
                str[c_i] = WS;
            }else{
                str[c_i] = c;
            }

            // Hämta nästa tecken, inkrementera index
            c = getc(corpus);
            c_i++;
        }

        str[c_i] = '\0';
        printf("...%s...\n", str);
 
        // Om vi nåt mer än 25 förekomster, fråga användaren om resterande
        // förekomster ska skrivas ut.
        if(i == 24)
            confirm_printout(num_occs, searchword);
    }

}

// Hanterar user input (kallas om antalet utskrifter överskrider 25)
static void confirm_printout(int num_occs, char *word){
            printf("\nDet finns %d förekomster av ordet \"%s\". \n\n", num_occs, word);
            printf("Vill du att resterande förekomster skrivs ut? Y / N \n\n>");
            while(1){
                char answer = getchar();
                
                switch(answer){
                    case 'Y':
                    case 'y':
                        return;
                    case 'N':
                    case 'n':
                        exit(1);
                    default:
                        printf("\nVänligen välj Y / N\n>");
                        getchar(); // Ät upp ENTER-char
                        break;
                }
            }
}
