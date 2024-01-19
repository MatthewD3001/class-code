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
	switch (*(char *)print) {
	    case TYPE_INT:
		print = (char *)print + 1;
		printf("%d", *(int *)print);
		print = (char *)print + sizeof((int *)print);
		break;

	    case TYPE_CHAR:
		print = (char *)print + 1;
		printf("%s", (char *)print);
		print = (char *)print + (int)strlen((char *)print) + 1;
		break;

	    case TYPE_FLOAT:
		print = (char *)print + 1;
		printf("%f", *(float *)print);
		print = (char *)print + sizeof((float *)print);
		break;

	    default:
		printf("mem is invalid at this address: %p\n", print);
		return;
	}
    }
    return;
}

void *f(int code, void * mem, void * data) {

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
    p = (char *)p + *(short int *)p;

    switch (code) {
	case F_data_int:
	    *(char *)p = TYPE_INT;
	    p = (char *)p + 1;
	    *(int *)p = *(int *)data;
	    *(short int *)mem += sizeof((int *)p) + 1;
	    break;

	case F_data_char:
	    *(char *)p = TYPE_CHAR;
	    p = (char *)p + 1;
	    strcpy((char *)p, (char *)data);
	    *(short int *)mem += strlen((char *)p) + 2;
	    break;

	case F_data_float: 
	    *(char *)p = TYPE_FLOAT;
	    p = (char *)p + 1;
	    *(float *)p = *(float *)data;
	    *(short int *)mem += sizeof((float *)p) + 1;
	    break;

	case F_print:
	    printmem(mem, p);
	    break;
    }
    return (void *)mem;
}
