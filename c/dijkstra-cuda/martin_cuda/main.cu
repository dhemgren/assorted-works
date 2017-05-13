#include "main.h"

int n, m;

int *v;
int *e;
int *w;

int *c;
int *f;
int *u;

int mssp;

int block_dim_minimum1;
int num_blocks_minimum1;

int block_dim_relax_update;
int num_blocks_relax_update;

// Pointers to device memory
int *vd;
int *ed;
int *wd;

int *cd;
int *fd;
int *ud;

__global__ void relax_helper(int *v, int *e, int *w, int *c, int *f, int *u, int n) {
    int index = (blockIdx.x * blockDim.x) + threadIdx.x;
    if (index > n-1)
        return;

    if (f[index]) {
        int start = v[index];
        int end = v[index+1];
        for (int j = start; j < end; j++) {
            int to = e[j];
            if (u[to]) {
                // Writes to c[to].
                atomicMin(&(c[to]), c[index] + w[j]);
            }
        }
    }
}

// relax_F
void relax(void) {
    // 1 thread block, n threads per block.
    relax_helper<<<num_blocks_relax_update, block_dim_relax_update>>>
        (vd, ed, wd, cd, fd, ud, n);
}

__global__ void minimum1_helper(int *c, const int *u, int *minimums, int n) {
    int out_of_bounds = false;
    int index = (blockIdx.x * blockDim.x) + threadIdx.x;
    if (index >= n)
        out_of_bounds = true;

    int thid = threadIdx.x;
    int *sdata = c + (blockIdx.x * blockDim.x);

    int i = blockIdx.x * (2 * blockDim.x) + threadIdx.x;
    int j = i + blockDim.x;

    int data1;
    int data2;

    if (i >= n)
        data1 = INT_MAX;
    if (i < n)
        data1 = u[i] ? c[i] : INT_MAX;

    if (j >= n)
        data2 = INT_MAX;
    if (j < n)
        data2 = u[j] ? c[j] : INT_MAX;

    if (!out_of_bounds)
        sdata[thid] = min(data1, data2);
    __syncthreads();
    for (int s = blockDim.x/2; s > 0; s>>=1) { // Dela s på 2 efter varje iteration
        if (thid < s && !out_of_bounds && index + s < n) {
            sdata[thid] = min(sdata[thid], sdata[thid+s]);
        }
        __syncthreads();
    }
    if (thid == 0 && !out_of_bounds) {
        minimums[blockIdx.x] = sdata[0];
    }
}

int minimum1(void) {
    //int num_threads_total = nearest_power_of_2(n) / 2;
    //int num_blocks = 1; // TODO: smartare val
    //int num_threads_per_block = num_threads_total / num_blocks;

    int *minimumsd;
    cudaMalloc((void **) &minimumsd, num_blocks_minimum1 * sizeof(int));

    int *cd_working_copy;
    cudaMalloc((void **) &cd_working_copy, n * sizeof(int));
    cudaMemcpy(cd_working_copy, cd, n * sizeof(int), cudaMemcpyDeviceToDevice);

    minimum1_helper<<<num_blocks_minimum1, block_dim_minimum1>>>(cd_working_copy, ud, minimumsd, n);

    int *minimums = (int *) malloc(num_blocks_minimum1 * sizeof(int));
    cudaMemcpy(minimums, minimumsd, num_blocks_minimum1 * sizeof(int), cudaMemcpyDeviceToHost);

    int res = INT_MAX;
    /* printf("MINIMUMS\n"); */
    for (int i = 0; i < num_blocks_minimum1; i++) {
        /* printf("%d\n", minimums[i]); */
        if (minimums[i] < res)
            res = minimums[i];
    }
    free(minimums);
    cudaFree(minimumsd);
    cudaFree(cd_working_copy);
    return res;
}

__global__ void update_helper(int *c, int *f, int *u, int mssp, int n) {
    int index = (blockIdx.x * blockDim.x) + threadIdx.x;
    if (index > n-1)
        return;

    f[index] = false;
    if (c[index] == mssp) {
        u[index] = false;
        f[index] = true;
    }
}

void update(void) {
    update_helper<<<num_blocks_relax_update, block_dim_relax_update>>>
        (cd, fd, ud, mssp, n);
}

void da2cf(void) {
    host_to_device();

    mssp = 0;
    int mssp_cu = 0;

    while (true) {
        relax();
        device_to_host(); // In order to calculate new mssp on host. To remove later.
        
        mssp = minimum_sequential();
        mssp_cu = minimum1();

        /* printf("%d %d\n", mssp, mssp_cu); */

        if (mssp_cu == INT_MAX)
            break;

        update();
    }

    device_to_host();
    print_res();
}

int main(void) {
    init();
    da2cf();
    free_memory();
}
