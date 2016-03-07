/* DTEK-Projektet, exempel-main*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "globals.h"  /* Egna projektspecifika deklarationer */

#define BTN2 1
#define BTN3 2
#define BTN4 4
#define TRUE 1
#define FALSE 0


struct bird {
    uint8_t x; // center x-koordinat
    uint8_t y; // center y-koordinat
	float velocity_y;
    uint8_t size;
};

struct pipe {
    int x; // nedre vänstra hörnet x
    int width;
	int hole_bottom;
	int hole_size;
    int active;
	int point_given;
};

enum screen {START, GAME, END};
enum screen current_screen = START;

volatile uint8_t current_state[4096];  // 128 * 32 = 4096 = pixlar på skärmen
volatile struct bird birdie;
volatile struct pipe pipes[POOL_SIZE];
volatile int delta_time = 0;
volatile int hole_bottoms[POOL_SIZE] = {3, 6, 9, 6, 8, 8, 4, 1, 7, 2, 7, 8, 5, 7, 7, 1, 6, 8, 10, 4, 10, 9, 7, 1, 6, 2, 4, 1, 3, 4, 3, 8, 2, 6, 3, 7, 6, 1, 6, 9, 4, 5, 4, 6, 5, 10, 4, 10, 8, 9, 4, 5, 3, 6, 2, 3, 7, 10, 8, 7, 3, 8, 6, 7, 4, 7, 7, 9, 8, 8, 8, 7, 10, 7, 4, 5, 6, 7, 8, 5, 9, 1, 7, 1, 4, 6, 8, 2, 3, 5, 5, 7, 2, 2, 2, 5, 9, 2, 3, 7};
volatile int holei = 0;
volatile int* trise = (volatile int*) 0xbf886100;
volatile int* ledaddr = (volatile int*) 0xbf886110;
volatile int points;
volatile int pipe_spawn_gap = PIPE_SPAWN_GAP;
volatile int speed_increase_counter = 0;

void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

void init(void){
	/* Set up peripheral bus clock */
    /* OSCCONbits.PBDIV = 1; */
    OSCCONCLR = 0x100000; /* clear PBDIV bit 1 */
	OSCCONSET = 0x080000; /* set PBDIV bit 0 */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

	display_init();

	*trise = *trise & ~0xFF; // Initialisera led-lamporna som output (d.v.s. LSB = 0x00)
}

/**
* Tillfällig timer-metod
*/
void Timer(void){
	quicksleep(10000); 
}

void update_leds(void){
  *ledaddr =  points;
}

uint8_t get_hole_height(int percent) {
	float res = (((float) percent)/10) * (32-HOLE_SIZE);

	return (uint8_t) res;
}

void init_game(void){
    birdie.x = 10;
    birdie.y = 15;
    birdie.size = 3;
	birdie.velocity_y = 0;
    pipe_spawn_gap = PIPE_SPAWN_GAP;
    speed_increase_counter = 0;

	int i;
	for (i = 0; i < POOL_SIZE; i++) {
		pipes[i].x = 128;
		pipes[i].width = PIPE_WIDTH;
		pipes[i].active = 0;
		pipes[i].hole_bottom = get_hole_height(hole_bottoms[i]);
		pipes[i].hole_size = HOLE_SIZE;
		pipes[i].point_given = FALSE;
	}
}

/**
* Fågelns aktiva flaxande (y-rörelse)
*/
int button_pressed(int btn){
    return (handle_buttons() == btn);
}

void SpawnPipe(void){
	pipes[holei].active = TRUE;
    ++holei;
    if(holei == 20)
        holei = 0;
}

/**
* Gravitation för fågeln, x-positionen för pipes
*/
void step_game(){

	if(button_pressed(BTN4))
		birdie.velocity_y = JUMP_FORCE;    
   	
	delta_time++;
	
	if(delta_time > pipe_spawn_gap){
		SpawnPipe();
		delta_time = 0;
        speed_increase_counter++;
	}
    
    if(speed_increase_counter > 9) {
        pipe_spawn_gap -= 5;
        speed_increase_counter = 0;
    }

	birdie.velocity_y -= GRAVITY;
	birdie.y = (uint8_t) (birdie.y + birdie.velocity_y);
    if (birdie.y > 31) {
        birdie.y = 31;
    }

	//int n = sizeof(pipes)/sizeof(pipes[0]);
	int i;
	for (i = 0; i < POOL_SIZE; i++) {
		if (pipes[i].active) {
			pipes[i].x -= SCROLL_SPEED;
			
			// ge poäng om birdie passerat pipe
			if ((pipes[i].x + pipes[i].width) < (birdie.x - (birdie.size-1) / 2) && !pipes[i].point_given) {
				++points;
				pipes[i].point_given = TRUE;
			}
			
			if (pipes[i].x + pipes[i].width < 0) {
				pipes[i].active = FALSE;
				pipes[i].x = 128;
				pipes[i].point_given = FALSE;
			}
		}
	}

	// TIDIGARE I RENDER
	/* Sätt hela displayen till 0 */
	fill_state(current_state, 0);

	/* Rita upp pipes */
	for (i = 0; i < POOL_SIZE; i++) {
		if (pipes[i].active) {
			draw_rect(current_state, pipes[i].x, 0, pipes[i].x + pipes[i].width, pipes[i].hole_bottom - 1, 1);
			draw_rect(current_state, pipes[i].x, pipes[i].hole_bottom + pipes[i].hole_size, pipes[i].x + pipes[i].width, 31, 1);
		}
	}

	/* Rita upp birdie */
    int coll = draw_rect_center(current_state, birdie.x, birdie.y, birdie.size, 1);
	
	if (coll) {
		current_screen = END;
		init_game();

		fill_state(current_state, 0);

		int i;
		int n = sizeof(end_screen) / sizeof(end_screen[0]);
		for (i = 0; i < n; i += 2) {
			set_point(current_state, end_screen[i] - 1, 32 - end_screen[i+1], 1);
		}
	}
}

void step_end() {
	if(button_pressed(BTN3)) {
        points = 0;
		current_screen = GAME;
    }
}

void step_start() {
	fill_state(current_state, 0);

	int i;
	int n = sizeof(start_screen) / sizeof(start_screen[0]);
	for (i = 0; i < n; i += 2) {
		set_point(current_state, start_screen[i] - 1, 32 - start_screen[i+1], 1);
	}

	if(button_pressed(BTN3))
		current_screen = GAME;
}

void Render(void){

	display_state(current_state, (uint8_t)SCREEN_WIDTH);

	update_leds();
}

int main(void) {	
	init();
    init_game();	

	while(TRUE) {
		switch (current_screen) {
			case START:
				step_start();
				break;
			case GAME:
				step_game();
				break;
			case END:
				step_end();
				break;
		}
		Render();
		quicksleep(100000);
	}
}


