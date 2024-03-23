#include <stdio.h>

int main() {
    int bit = 0;
    unsigned char byte = 4;
    printf("right shift 1 & with 1: %d\n", ((byte >> 1) & 1));
    printf("right shift 2 & with 1: %d\n", ((byte >> 2) & 1));
}
