#include <stdio.h>

int* bad_stack_ptr(void) {
    int x = 123;
    printf("inside bad_stack_ptr: &x = %p, x=%d\n", (void*)&x, x);
    return &x; // ❌ returning address of a local (stack) variable
}

int main(void) {
    int *p = bad_stack_ptr();
    printf("in main: p = %p\n", (void*)p);
    printf("in main: *p = %d (undefined behavior)\n", *p); // may print garbage or crash
    return 0;
}
