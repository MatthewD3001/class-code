#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define  F_first        1   /* This is the first call to the function. */
#define  F_last         2   /* This is the last call to the function. Free the memory area. */
#define  F_data_int     3   /* Void * argument points to integer data. */
#define  F_data_char    4   /* Void * argument points to character string. */
#define  F_data_float   5   /* Void * argument points to a float data value. */
#define  F_print        6   /* Print the accumulated values. */

void * f (int code, void * mem, void * data) {

    long long int z_data = (long long int)data;

    if (code == F_first) {
	if((size_t)data != 0){
	    printf("set pointer\n");
	    return malloc((size_t)data);
	} else {
	    printf("failed to set pointer\n");
	    return (void *)0;
	}
    } else if (code == F_last) {
	free(mem);
	return (void *)0;
    }
    char *p = (char *)mem;
    printf("Current pointer: %p\n", p);
    p = p + 2;
    printf("new pointer: %p\n\n", p);


    switch (code) {
	case F_data_int:
	    //implement
	    break;
	case F_data_char:
	    //implement
	    break;
	case F_data_float:
	    //implement
	    break;
	case F_print:
	    printf("%d : %p %p %lld\n",code, mem, data, z_data);
	    break;
    }

    return (void *)mem;
}
