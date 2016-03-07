#include <stdint.h>
#include <pic32mx.h>

#define BTN2 1
#define BTN3 2
#define BTN4 4

int getsw (void) {
	return (PORTD & 0xF00) >> 8;
}

int getbtns(void) {
	return (PORTD & 0xE0) >> 5;
}

int handle_buttons(void){
  int switches = getsw();
  int buttons = getbtns();
  
  switch(buttons){
    case BTN2:
      return BTN2;
      break;

    case BTN3:
      return BTN3;
      break;

    case BTN4:
      return BTN4;
      break;

    default:
      break;
  }
}
