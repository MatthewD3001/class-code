#include <stdlib.h>
#include <stdio.h>

int main() {
    int A[] = {1,2,3,4,5,6,7,8,9,10};
    int B[] = {1,2,3,4,5,6,7,8,9,10};
    int n = 10;
    int m = -1;
    int i = 0;
    int j = n - 1;
    int currSum = 0;
    while(i < n && j >= 0) {
        currSum = (2 * B[j]) - (3 * A[i]) - 6;
        if (currSum == m) {
            printf("True, correct numbers are: A[%d] = %d and B[%d] = %d\n", i, A[i], j, B[j]);
            exit(EXIT_SUCCESS);
        } else if (currSum < m) {
            i++;
        } else if (currSum > m) {
            j--;
        }
    }
    printf("False\n");
}
