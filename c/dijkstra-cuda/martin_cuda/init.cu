#include "main.h"

static void scan_header(void) {
    char first;
    while (true) {
        first = getchar();
        if (first != 'c')
            break;
        scanf("%*[^\n]\n");
    }

    // Scan number of verts and edges
    scanf(" sp %d %d\n", &n, &m);
}

static void scan_edges(void) {
    for (int i = 0; i < n+1; i++) {
        v[i] = -1;
    }
    
    // Scan edges
    int prev_from, curr_from, curr_to, curr_weight;
    prev_from = -1;
    for (int i = 0; i < m; i++) {
        scanf("a %d %d %d\n", &curr_from, &curr_to, &curr_weight);
        //printf("%d %d %d\n", curr_from, curr_to, curr_weight);
        //printf("i: %d, from: %d, to: %d, weight: %d\n", i, curr_from, curr_to, curr_weight);
        
        if (curr_from != prev_from) {
            v[curr_from] = i;
            prev_from = curr_from;
        }
        
        e[i] = curr_to;
        w[i] = curr_weight;
    }
    
    //printf("v innan sista genomgång\n");
    //for (int i = 0; i < n+1; i++) {
    //    printf("%d, ", v[i]);
    //}
    //printf("\n\n");
    
    v[n] = m;
    
    for (int i = n-1; i >= 0; i--) {
        if (v[i] == -1) {
            v[i] = v[i+1];
        }
    }     
}

static void calculate_block_dimensions(void){
    // minimum1
    int num_threads_total = nearest_power_of_2(n)/2;
    if(num_threads_total < 32)
        num_threads_total = 32;

    num_blocks_minimum1 = num_threads_total / 32;
    block_dim_minimum1 = num_threads_total / num_blocks_minimum1;
    /* printf("num_blocks_minimum1: %d\n", num_blocks_minimum1); */
    /* printf("block_dim_minimum1: %d\n", block_dim_minimum1); */
    // Multiplar av 32 

    // relax och update
    num_threads_total = nearest_power_of_2(n);
    if(num_threads_total < 32)
        num_threads_total = 32;

    num_blocks_relax_update = num_threads_total / 32;
    block_dim_relax_update = num_threads_total / num_blocks_relax_update;
    /* printf("num_blocks_relax_update: %d\n", num_blocks_relax_update); */
    /* printf("block_dim_relax_update: %d\n", block_dim_relax_update); */
}

void init(void) {
    scan_header();
    allocate_memory();
    scan_edges();
    calculate_block_dimensions();

    // Initialize c, f, and u
    for (int i = 0; i < n; i++) {
        c[i] = INT_MAX;
        f[i] = false;
        u[i] = true;
    }
    c[SRC] = 0;
    f[SRC] = true;
    u[SRC] = false;
}
