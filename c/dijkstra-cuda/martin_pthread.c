#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <pthread.h>
#include "main.h"

#define SRC 0

int n, m;

int *v;
int *e;
int *w;

int *c;
int *f;
int *u;

int mssp;
int work_mssp; // Variable worked on in parallel minimum routine

pthread_mutex_t lock_c = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_work_mssp = PTHREAD_MUTEX_INITIALIZER;

int min(int a, int b) {
    if (a > b)
        return b;
    else
        return a;
}

// Launch n threads running start_routine with distinct index values as
// arguments, and wait for them to finish.
void parallel_do(void *(*start_routine)(void *), int n) {
    pthread_t threads[n];
    for (int i = 0; i < n; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&(threads[i]), NULL, start_routine, arg);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
}

void *relax_helper(void *arg) {
    int i = *((int *) arg);
    if (f[i]) {
        int start = v[i];
        int end = v[i+1];
        for (int j = start; j < end; j++) {
            int to = e[j];
            if (u[to]) {
                pthread_mutex_lock(&lock_c);
                c[to] = min(c[to], c[i] + w[j]);
                pthread_mutex_unlock(&lock_c);
            }
        }
    }
    free(arg);
    return NULL;
}

// relax_F
void relax(void) {
    parallel_do(relax_helper, n);
}

void *minimum_helper(void *arg) {
    int i = *((int *) arg);
    if (!u[i]) { // For every frontier vertex i
        int start = v[i];
        int end = v[i+1];
        for (int j = start; j < end; j++) {
            int to = e[j];
            if (u[to]) {
                pthread_mutex_lock(&lock_work_mssp);
                work_mssp = min(work_mssp, c[to]);
                pthread_mutex_unlock(&lock_work_mssp);
            }
        }
    }
    free(arg);
    return NULL;
}

int minimum(void) {
    work_mssp = INT_MAX;
    parallel_do(minimum_helper, n);
    return work_mssp;
}

void *update_helper(void *arg) {
    int i = *((int *) arg);
    f[i] = false;
    if (c[i] == mssp) {
        u[i] = false;
        f[i] = true;
    }
    free(arg);
    return NULL;
}

void update(void) {
    parallel_do(update_helper, n);
}

void init(void) {
    for (int i = 0; i < n; i++) {
        c[i] = INT_MAX;
        f[i] = false;
        u[i] = true;
    }
    c[SRC] = 0;
    f[SRC] = true;
    u[SRC] = false;
}

void da2cf(void) {
    init();
    mssp = 0;

    while (true) {
        relax();
        mssp = minimum();

        // Branch divergence issue?
        if (mssp == INT_MAX)
            break;

        update();
    }
}

void print_result(){
    for (int i = 0; i < n; i++) {
        printf("%d %d\n", i, c[i]);
    }
}

int main(void) {
    scan();
    da2cf();
    print_result();
}
