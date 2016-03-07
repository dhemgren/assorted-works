/* DTEK-Projektet, hanterar spelvärlden*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "globals.h"


/* 1 = aktiv pixel, 0 = ej aktiv pixel */
volatile uint8_t current_state[4096];

uint8_t roller = 2;

void fill_state(uint8_t state[], uint8_t x){
	int i;
	for(i = 0; i < 4096; i++)
		state[i] = x;
}

void fill_state_modulo(uint8_t state[], uint8_t mod, uint8_t x){
	int i;

	for(i = 0; i < 4096; i++)
		if(i % mod == 0)
			state[i] = 0;
		else
			state[i] = x;
}

void fill_state_roller(uint8_t state[], uint8_t x){
	int i;
	for(i = 1; i < 4096; i += roller){
		state[i] = x;
	}
	roller++;
}


int set_point(volatile uint8_t state[], uint8_t x, uint8_t y, uint8_t value){
	int coll = 0;    
	if (y >=0 && y <= 31 && x >= 0 && x <= 127) {
		coll = state[SCREEN_HEIGHT * x + (31 - y)];
	    state[SCREEN_HEIGHT * x + (31 - y)] = value;
	}
	return coll;
}

int draw_rect(volatile uint8_t state[], int xleft, int ybottom, int xright, int ytop, uint8_t value){
    int y;
    int x;

	int colltemp;
	int coll = 0;

    for(y = ybottom; y <= ytop; y++) {
        for(x = xleft; x <= xright; x++) {
			colltemp = set_point(state, (uint8_t) x, (uint8_t) y, value);
			if (colltemp)
				coll = 1;
		}
    }

	return coll;
}

int draw_rect_center(volatile uint8_t state[], int x, int y, uint8_t size, uint8_t value) {
	int factor = (size-1)/2;
	return draw_rect(state, x-factor, y-factor, x+factor, y+factor, value);
}
