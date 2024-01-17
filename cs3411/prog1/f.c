#include <unistd.h>
#include <stdio.h>

#define  F_first        1   /* This is the first call to the function. */
#define  F_last         2   /* This is the last call to the function. Free the memory area. */
#define  F_data_int     3   /* Void * argument points to integer data. */
#define  F_data_char    4   /* Void * argument points to character string. */
#define  F_data_float   5   /* Void * argument points to a float data value. */
#define  F_print        6   /* Print the accumulated values. */

void * f (int code, void * mem, void * data)
{
    long long int z_data = (long long int)data;

    printf("%d : %p %p %lld\n",code, mem, data, z_data);
    return mem;
}
