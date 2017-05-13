#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>

#define SRC 0

extern int n, m;

extern int *v;
extern int *e;
extern int *w;

extern int *c;
extern int *f;
extern int *u;

extern int mssp;

extern int block_dim_minimum1;
extern int num_blocks_minimum1;

extern int block_dim_relax_update;
extern int num_blocks_relax_update;

extern int *vd;
extern int *ed;
extern int *wd;

extern int *cd;
extern int *fd;
extern int *ud;

// init.cu
void init(void);

// util.cu
void print_c_u(void);
void print_res(void);
int  nearest_power_of_2(int);
void host_to_device(void);
void device_to_host(void);
void allocate_memory(void);
void free_memory(void);

// legacy.cu
int my_min(int, int);
int minimum_sequential(void);
