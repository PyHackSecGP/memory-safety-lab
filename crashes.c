#include <stdio.h>
#include <stdlib.h>

void null_deref(void) {
    int *p = NULL;
    *p = 123; // segfault
}

void oob_stack(void) {
    int a[4] = {0};
    a[1000] = 7; // may segfault or corrupt silently (UB)
}

void stack_overflow(void) {
    char big[1024 * 1024]; // 1MB
    big[0] = 1;
    stack_overflow();      // recurse until stack dies
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s {1|null,2|oob,3|stack}\n", argv[0]);
        return 0;
    }
    int mode = atoi(argv[1]);
    if (mode == 1) null_deref();
    if (mode == 2) oob_stack();
    if (mode == 3) stack_overflow();
    return 0;
}
