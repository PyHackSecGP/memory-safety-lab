#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *p = malloc(16);
    if (!p) return 1;

    strcpy(p, "hello");
    free(p);

    // Intentional UAF: write after free
    p[0] = 'H';

    // allocator activity to encourage reuse
    char *q = malloc(16);
    if (!q) return 1;
    strcpy(q, "world");

    printf("%s %s\n", p, q);

    free(q);
    return 0;
}
