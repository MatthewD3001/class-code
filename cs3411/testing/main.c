#include <unistd.h>
#include <stdio.h>

#define  F_first        1   /* This is the first call to the function. */
#define  F_last         2   /* This is the last call to the function. Free the memory area. */
#define  F_data_int     3   /* Void * argument points to integer data. */
#define  F_data_char    4   /* Void * argument points to character string. */
#define  F_data_float   5   /* Void * argument points to a float data value. */
#define  F_print        6   /* Print the accumulated values. */

void * f (int code, void * mem, void * data);

int main() {
    int a = 15;
    int *ap = &a;
    float b = 17.4325;
    float *bp = &b;

    void *m = 0;

    m = f(F_first, 0, (void *)200);

    m = f(F_data_int, m, (void *)ap);
    m = f(F_data_char, m, (void *)" people are in the ");
    m = f(F_data_char, m, (void *)"systems pro");
    m = f(F_data_char, m, (void *)"gramming class. ");
    m = f(F_data_char, m, (void *)"If you add 2.4325 to that then you get ");
    m = f(F_data_float, m, (void *)bp);
    m = f(F_data_char, m, (void *)".");
    m = f(F_data_char, m, (void *)"\n");

    m = f(F_print, m, 0);
    m = f(F_last, m, 0);
}
