#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define  F_first        1   /* This is the first call to the function. */
#define  F_last         2   /* This is the last call to the function. Free the memory area. */
#define  F_data_int     3   /* Void * argument points to integer data. */
#define  F_data_char    4   /* Void * argument points to character string. */
#define  F_data_float   5   /* Void * argument points to a float data value. */
#define  F_print        6   /* Print the accumulated values. */
#define	 TYPE_INT	7   // More legible name for the int type.
#define	 TYPE_CHAR	8   // More legible name for the char type.
#define	 TYPE_FLOAT	9   // More legible name for the float type.

/**
 * This is a helper function to return the length of a string INCLUDING the null byte.
 *
 * Param input: this is the input char pointer for the beginning of a string.
 */
int mystrlen(char *input) {
    char *begin = input;
    while(*input++);
    return input - begin;
}

/**
 * This is a helper function to copy one string to another, note there are not saftey checks here. If the user passes
 * a non null terminated string this will run indefinetily.
 *
 * Param target: this is the target pointer to be copied to.
 * Param source: this is the source of the copied string.
 */
void mystrcpy(char *target, char *source) {
    do *target++ = *source;
    while(*source++);
    return;
}

/**
 * This is a helper function to simply print out the contents of the mem pointer.
 *
 * Param mem: This is the pointer for the whole data structure, pointing to it's first byte of data.
 * Param p: This is the pointer to the end of the data structure so that the print knows when to stop.
 */
void printmem(void *mem, void *p){

    void *print = (char *)mem + 2;  // Skip the first two bytes as those contain the offset, not data.
    
    /**
     * This loop continues until the printing pointer has reached the end of the data within mem.
     *
     * Inside is a switch statement which reads the first byte, this contains the type of the data stored in the following space.
     *
     * Each case first moves to the data segment of the chunk of memory.
     * Then it properly prints the data stored depending on its type.
     * Finally it increments the print pointer to the next type identifying byte.
     *
     * The default statement points out where data was stored improperly or this function was used improperly.
     */
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
		print = (char *)print + mystrlen((char *)print);
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

/**
 * This is the primary function for creating, inserting, and parsing data within this data structure.
 *
 * Param code: This tells the function what type of action to do such as initiallize, close, insert, or print data.
 * Param mem: This is the pointer to the current data structure if one exists.
 * Param data: Can contain any data type this function can use: int *, char *, float *
 *	       Additionally can contain the number of bytes to allocate.
 */
void * f(int code, void * mem, void * data) {

    void *p = 0;			// Main pointer to modify data in this data structure.

    /**
     * First function call, check data to know how much space to allocate.
     * Allocate memory, set the first two bytes to empty data offset, return allocated pointer.
     */
    if (code == F_first) {
	if((size_t)data != 0){
	    p = malloc((size_t)data);
	    *(short int *)p = 2;
	    return (void *)p;
	} else {
	   return p;
	}
    } else if (code == F_last) {
	free(mem);
	return p;
    }

    if(!mem){				// Make sure the user passed a valid pointer.
	printf("Initiallize mem!\n");
	return p;
    }

    p = mem;
    p = (char *)p + *(short int *)p;	// This reads the first two bytes for the short int which is equal to the number 
					// of bytes needed to move in order to be at the first empty data location.

    /**
     * This switch statement takes the code passed to the function and loads the associated data properly.
     *
     * In each case it will first assign the first byte, already at the right location from line above,
     * to the type that will be stored so it can be identified. Then it shifts by 1 byte and inserts the data.
     *
     * After this, each case will go back to the original pointer and reassign the first two bytes to contain
     * the proper offset value with the new data inserted.
     */
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
	    mystrcpy((char *)p, (char *)data);
	    *(short int *)mem += mystrlen((char *)p) + 1;
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
