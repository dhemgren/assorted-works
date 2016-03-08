/* DTEK-Projektet, exempel-main*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "globals.h"

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}


void display_state(const volatile uint8_t state[], uint8_t width) {

	int i, j;
	uint8_t current_row, row_y, x, 
		current_pixel, current_pixel_tot, pixel_height, x0;
	

	for(current_row = 0; current_row < 4; current_row++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0xA7); // Invertera skärm för debugging


		spi_send_recv(0x22); 		// Set page address = 
		spi_send_recv(current_row); // current page (row)
		
		spi_send_recv(0x00); // Set lower nibble start pos.
		spi_send_recv(0x10); // Set upper nibble start pos.

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(x = 0; x < width; x++){
			current_pixel_tot = 0;
			pixel_height = 0;

			for(row_y = current_row*8; row_y < current_row*8+8; row_y++){
				current_pixel = (uint8_t)state[x * SCREEN_HEIGHT + row_y];
				current_pixel_tot = current_pixel_tot | current_pixel << pixel_height; //(7 - pixel_height)?? 
				pixel_height++;
			}

			spi_send_recv(current_pixel_tot);	

		}
	}
}


void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		
		spi_send_recv(0x22); // Set page address = 
		spi_send_recv(i);	 // current page
		
		/* Address på 1 byte som skickas som två nibbles: */
		spi_send_recv(x & 0xF); // Set lower column start address
		spi_send_recv(0x10 | ((x >> 4) & 0xF)); // Set higher column start address
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++)
			spi_send_recv(~data[i*32 + j]);
	}
}


void entire_screen_on(){
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	spi_send_recv(0xA5);
	DISPLAY_CHANGE_TO_DATA_MODE;
}

void entire_screen_resume(){
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	spi_send_recv(0xA4);
	DISPLAY_CHANGE_TO_DATA_MODE;
}

void clear_screen(){
		int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++)
			spi_send_recv(255);
	}

}

void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1); // Set vertical addressing mode ??
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);

	spi_send_recv(0x20); // Set memory addressing mode ??
	
	spi_send_recv(0xAF); // Set display ON
}


