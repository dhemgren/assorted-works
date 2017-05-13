#include <stdio.h>
#include <stdlib.h>
#include "main.h"

void scan(void) {
    // Scan number of verts and edges
    scanf("p sp %d %d\n", &n, &m);

    v = malloc((n+1) * sizeof(int));
    e = malloc(m * sizeof(int));
    w = malloc(m * sizeof(int));

    c = malloc(n * sizeof(int));
    f = malloc(n * sizeof(int));
    u = malloc(n * sizeof(int));

    int **edges = malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        edges[i] = malloc(3 * sizeof(int));
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < 3; j++) {
            edges[i][j] = 0;
        }
    }

    // För varje kant
    for(int i = 0; i < m; i++) {
        int from, to, weight;
        scanf("a %d %d %d\n", &from, &to, &weight);
        edges[i][0] = from;
        edges[i][1] = to; 
        edges[i][2] = weight;
    }

    int e_index = 0;
    // För varje hörn
    for (int i = 0; i < n; i++) {
        v[i] = e_index;
        // För varje kant
        for (int j = 0; j < m; j++) {
            if (edges[j][0] == i) {
                e[e_index] = edges[j][1];
                w[e_index] = edges[j][2];
                e_index++;
            }
        }
    }
    v[n] = m;
}
