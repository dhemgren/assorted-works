#include "main.h"

int my_min(int a, int b) {
    if (a > b)
        return b;
    else
        return a;
}

int minimum_sequential(void) {
    int new_mssp = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (!u[i]) { // For every frontier vertex i
            int start = v[i];
            int end = v[i+1];
            for (int j = start; j < end; j++) {
                int to = e[j];
                if (u[to]) {
                    new_mssp = my_min(new_mssp, c[to]);
                }
            }
        }
    }
    return new_mssp;
}

