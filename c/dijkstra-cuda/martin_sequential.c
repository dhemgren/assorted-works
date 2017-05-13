#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
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

int min(int a, int b) {
    if (a > b)
        return b;
    else
        return a;
}

// relax_F
void relax(void) {
    for (int i = 0; i < n; i++) {
        if (f[i]) {
            int start = v[i];
            int end = v[i+1];
            for (int j = start; j < end; j++) {
                int to = e[j];
                if (u[to]) {
                    c[to] = min(c[to], c[i] + w[j]);
                }
            }
        }
    }
}

int minimum(void) {
    int new_mssp = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (!u[i]) { // For every frontier vertex i
            int start = v[i];
            int end = v[i+1];
            for (int j = start; j < end; j++) {
                int to = e[j];
                if (u[to]) {
                    new_mssp = min(new_mssp, c[to]);
                }
            }
        }
    }
    return new_mssp;
}

void update(void) {
    for (int i = 0; i < n; i++) {
        f[i] = false;
        if (c[i] == mssp) {
            u[i] = false;
            f[i] = true;
        }
    }
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
