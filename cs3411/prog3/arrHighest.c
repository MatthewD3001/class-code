#include <stdio.h>
#include <stdlib.h>

int** highestValues(int *arr, int count) {
    int *out = (int *)malloc(sizeof(int) * count);
    out[0] = 1;
    out[1] = 2;
    out[2] = 3;
    out[3] = 4;
    out[4] = 5;
    return &out;
}

int main() {
    int a[] = {1,2,3,4,5,6,7};
    int **b;
    b = highestValues(a, 7);
    printf("%d\n", b[4]);
    printf("size of returned array: %d\n", sizeof(b));
    printf("size of returned array: %d\n", sizeof(*b));
}
