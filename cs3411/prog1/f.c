#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  F_first        1   /* This is the first call to the function. */
#define  F_last         2   /* This is the last call to the function. Free the memory area. */
#define  F_data_int     3   /* Void * argument points to integer data. */
#define  F_data_char    4   /* Void * argument points to character string. */
#define  F_data_float   5   /* Void * argument points to a float data value. */
#define  F_print        6   /* Print the accumulated values. */
#define	 TYPE_INT	7
#define	 TYPE_CHAR	8
#define	 TYPE_FLOAT	9

void printmem(void *mem, void *p){
    void *print = (char *)mem + 2;
    while(print < p){

	print = (char *)print + 1;
    }
    printf("\np pointer: %p\nprint pointer: %p\n", p, print);
    return;
}

void * f (int code, void * mem, void * data) {

    //long long int z_data = (long long int)data;

    void *p = 0;
    if (code == F_first) {
	if((size_t)data != 0){
	    p =  malloc((size_t)data);
	    *(short int *)p = 2;
	    return (void *)p;
	} else {
	    return p;
	}
    } else if (code == F_last) {
	free(mem);
	return p;
    }

    if(!mem){
	printf("Initiallize mem!\n");
	return p;
    }

    p = mem;

    printf("Current pointer: %p\n", p);
    printf("Offset: %d\n", *(short int *)p);
    p = (void *)((char *)p + *(short int *)p);
    printf("Moved pointer: %p\n\n", p);

    switch (code) {
	case F_data_int:
	    printf("Adding INT to mem!\n");
	    *(char *)p = TYPE_INT;
	    p = (char *)p + 1;
	    p = (int *)data;

	    //printf("What data holds: %d\n", *(int *)data);
	    //printf("What mem holds: %d\n", *(int *)p);
	    //printf("Size of data: %ld\n", sizeof((int *)data));
	    //printf("Size of mem at location: %ld\n", sizeof((int *)p));

	    *(short int *)mem += sizeof((int *)p);

	    break;

	case F_data_char:
	    printf("Adding CHAR to mem!\n");
	    *(char *)p = TYPE_CHAR;
	    p = (char *)p + 1;
	    p = (char *)data;

	    //printf("What data holds: %s\n", (char *)data);
	    //printf("What mem holds: %s\n", (char *)p);
	    //printf("Size of data: %ld\n", strlen((char *)data) + 1);
	    //printf("Size of mem at location: %ld\n", strlen((char *)p) + 1);

	    *(short int *)mem += strlen((char *)p) + 1;
	    
	    break;

	case F_data_float: 
	    printf("Adding FLOAT to mem!\n");
	    *(char *)p = TYPE_FLOAT;
	    p = (char *)p + 1;
	    p = (float *)data;

	    //printf("What data holds: %f\n", *(float *)data);
	    //printf("What mem holds: %f\n", *(float *)p);
	    //printf("Size of data: %ld\n", sizeof((float *)data));
	    //printf("Size of mem at location: %ld\n", sizeof((float *)p));

	    *(short int *)mem += sizeof((float *)p);

	    break;

	case F_print:
	    printmem(mem, p);
	    break;
    }

    return (void *)mem;
}
