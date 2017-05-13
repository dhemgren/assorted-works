#include "main.h"

void print_c_u(void) {
    printf("\n\nu:\n");
    for(int i = 0; i < n; i++){
        printf("u[%d]=%d\n", i, u[i]);
    }

    printf("\n\nc:\n");
    for(int i = 0; i < n; i++){
        printf("c[%d]=%d\n", i, c[i]);
    }
}

void print_res(void) {
    for (int i = 0; i < n; i++) {
        printf("%d %d\n", i, c[i]);
    }
}

int nearest_power_of_2(int arg) {
    unsigned int x = (unsigned int) arg;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++; 
    return x;
}

void host_to_device(void) {
    cudaMemcpy(vd, v, (n+1) * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(ed, e, m * sizeof(int),     cudaMemcpyHostToDevice);
    cudaMemcpy(wd, w, m * sizeof(int),     cudaMemcpyHostToDevice);

    cudaMemcpy(cd, c, n * sizeof(int),     cudaMemcpyHostToDevice);
    cudaMemcpy(fd, f, n * sizeof(int),     cudaMemcpyHostToDevice);
    cudaMemcpy(ud, u, n * sizeof(int),     cudaMemcpyHostToDevice);
}

void device_to_host(void) {
    cudaMemcpy(v, vd, (n+1) * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(e, ed, m * sizeof(int),     cudaMemcpyDeviceToHost);
    cudaMemcpy(w, wd, m * sizeof(int),     cudaMemcpyDeviceToHost);

    cudaMemcpy(c, cd, n * sizeof(int),     cudaMemcpyDeviceToHost);
    cudaMemcpy(f, fd, n * sizeof(int),     cudaMemcpyDeviceToHost);
    cudaMemcpy(u, ud, n * sizeof(int),     cudaMemcpyDeviceToHost);
}

// Precondition: n and m are set
void allocate_memory(void) {
    v = (int *) malloc((n+1) * sizeof(int));
    e = (int *) malloc(m * sizeof(int));
    w = (int *) malloc(m * sizeof(int));
    c = (int *) malloc(n * sizeof(int));
    f = (int *) malloc(n * sizeof(int));
    u = (int *) malloc(n * sizeof(int));

    assert(cudaMalloc((void **) &vd, (n+1) * sizeof(int)) == cudaSuccess);
    assert(cudaMalloc((void **) &ed, m * sizeof(int)) == cudaSuccess);
    assert(cudaMalloc((void **) &wd, m * sizeof(int)) == cudaSuccess);
    assert(cudaMalloc((void **) &cd, n * sizeof(int)) == cudaSuccess);
    assert(cudaMalloc((void **) &fd, n * sizeof(int)) == cudaSuccess);
    assert(cudaMalloc((void **) &ud, n * sizeof(int)) == cudaSuccess);
}

// Precondition: allocate_memory has ran
void free_memory(void) {
    cudaFree(vd);
    cudaFree(ed);
    cudaFree(wd);
    cudaFree(cd);
    cudaFree(fd);
    cudaFree(ud);

    free(v);
    free(e);
    free(w);
    free(c);
    free(f);
    free(u);
}
