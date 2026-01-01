#include <stdio.h>
#include <stdlib.h>

void stack_demo(void) {
    int x = 42;                 // stack
    printf("stack x addr: %p value=%d\n", (void*)&x, x);
}

void heap_demo(void) {
    int *p = malloc(sizeof(int));  // heap
    if (!p) exit(1);
    *p = 1337;
    printf("heap p addr: %p value=%d\n", (void*)p, *p);
    free(p);
}

int main(void) {
    stack_demo();
    heap_demo();
    return 0;
}
