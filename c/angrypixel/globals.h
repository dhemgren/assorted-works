/*Global definitions */ 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

/* "Config file" */
#define JUMP_FORCE 1.5
#define SCROLL_SPEED 1
#define PIPE_WIDTH 5
#define PIPE_SPAWN_GAP 50
#define GRAVITY 0.08
#define HOLE_SIZE 15
#define POOL_SIZE 100

/*Declare gameworld-related functions*/
uint8_t * get_state();

/*Declare display-related functions*/
void display_state(const volatile uint8_t state[], uint8_t width);
void display_image(int x, const uint8_t *data);
uint8_t spi_send_recv(uint8_t data);


/*Game world declarations*/
volatile uint8_t current_state[4096];
int set_point(volatile uint8_t state[], uint8_t x, uint8_t y, uint8_t value);

/*GFX declarations*/
extern const uint8_t icon[128];
extern const uint8_t start_screen[320];
extern const uint8_t end_screen[316];

/*input.c*/
int handle_buttons(void);

