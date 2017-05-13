#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "konkordans.h"

unsigned int prefix_hash(char *in_str) {
    unsigned int res;
    uint8_t formatted_prefix[3];
    uint8_t packed_prefix[3];

    format_prefix(in_str, formatted_prefix);
    pack_prefix(formatted_prefix, packed_prefix);

    res = 900 * packed_prefix[0] + 30 * packed_prefix[1] + 1 * packed_prefix[2];
    //    └30^2

    /* printf("Hash function ran with:\nInput string: \"%s\"\nReturn value: %u\n\n", */
    /*         in_str, res); */
    return res;
}

// Takes a string of arbitrary length ("in") and returns an uint8_t array of
// length 3 ("out"). If in is shorter than 3, 0 are placed in the empty spots.
void format_prefix(char *in, uint8_t *out) {
    int len = strlen(in);

    for (int i = 0; i < 3; i++) {
        if (i < len) {
            out[i] = in[i];
        } else  {
            out[i] = (uint8_t) 0;
        }
    }
}

// Shifts numbers in array "in" so that ISO latin 1 codes for a...ö
// are represented by numbers 1...29. Stores the result in array "out".
void pack_prefix(uint8_t *in, uint8_t *out) {
    for (int i = 0; i < 3; i++) {
        if (97 <= in[i] && in[i] <= 122) { // a-z
           out[i] = in[i] - 96; // a = 1, b = 2, ... 
        } else {
            switch (in[i]) {
                case 228: // ä
                    out[i] = 27;
                    break;
                case 229: // å
                    out[i] = 28;
                    break;
                case 246: // ö
                    out[i] = 29;
                    break;
                case 0: // om prefix är kortare än 3 har vi lagt in nollor
                    out[i] = 0;
                    break;
            }
        }

    }
}
