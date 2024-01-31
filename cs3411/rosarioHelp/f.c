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

void * f (int code, void * mem, void * data)
{
    long long int z_data = (long long int)data;

    short *offset;

    switch(code) {
        case F_first:
            printf("F_first\n");
            if (data == 0){
                return 0;
            }
            mem = (void *)malloc(z_data);
            offset = (short *)mem;
            *offset = 2;
            printf("%d : %p %p %lld\n",code, (char *)mem, data, z_data);
            break;

        case F_data_int:
            printf("Adding an int\n");
            //Read the offset
            offset = (short *)mem;
            printf("Read offset: %d\n", *offset);
            //Increment mem by offset bytes
            char *local_mem = (char *)mem;
            local_mem = local_mem + *offset;
            printf("Increment mem\n local_mem: %p\n mem: %p\n", local_mem, mem);
            //local_mem contains a pointer to the beginning of free space
            //Write data type
            *local_mem = F_data_int;
            local_mem++;
            //Write the int into empty space
            *((int *)local_mem) = *((int*)data);
            *offset += 5;
            printf("%d : %p %p %lld %p\n",code, mem, data, z_data, local_mem);
            break;
        
        case F_data_char:
            printf("Adding a char\n");
            //Read the offset
            offset = (short *)mem;
            printf("Read offset: %d\n", *offset);
            //Increment mem by offset bytes
            local_mem = (char *)mem;
            local_mem += *offset;
            printf("Increment mem\n local_mem: %p\n mem: %p\n", local_mem, mem);
            *local_mem = F_data_char;
            local_mem++;
            *offset = *offset + 1;
            while (*(char*)data != '\0') {
                *local_mem = *(char *)data;
                local_mem++;
                data = (char*)data + 1;
                *offset = *offset + 1;
            }
            *local_mem = *(char *)data;
            *offset = *offset + 1;
            printf("%d : %p %p %lld %p\n",code, mem, data, z_data, local_mem);
            break;

        case F_data_float:
            printf("Adding a float\n");
            //Read the offset
            offset = (short *)mem;
            //Increment mem by offset bytes
            local_mem = (char *)mem;
            local_mem = local_mem + *offset;
            //local_mem contains a pointer to the beginning of free space
            //Write data type
            *local_mem = F_data_float;
            local_mem++;
            //Write the int into empty space
            *((float *)local_mem) = *((float*)data);
            *offset += 5;
            printf("%d : %p %p %lld %p\n",code, mem, data, z_data, local_mem);
            break;

        case F_print:
            printf("Printing\n");
            //Start pointer at 2 to ignore first 2 bytes
            int pointer = 2;
            //Read the offset
            offset = (short *)mem;
            void *print = (char *)mem + 2;
            while((char *)print < (char *)offset){
                switch (*(char *)print) {
                    case 3:
                        printf("Printing int");
                        break;
                    case 5:
                        printf("Printing string");
                        break;
                    case 4:
                        printf("Printing float");
                        break;
                    default:
                        //printf("mem invalid at: %p", print);
                }
//                if (*(char*)((char*)mem + pointer) == 3){
//                    pointer++;
//                    //printf("%d", );
//                    pointer = pointer + 4;
//                }
//                else if (*(char*)((char*)mem + pointer) == 5){
//                    pointer++;
//                }
//                else if (*(char*)((char*)mem + pointer) == 4){
//                    pointer++;
//                    //printf("%f", );
//                    pointer = pointer + 4;
//                }
//                else {
//                    printf("mem invalid at: %p\n", (char *)mem + pointer);
//                    pointer += 9999999;
//                }
            }
            break;

        case F_last:
            printf("Freeing mem\n");
            free(mem);
            break;
    }
    return mem;
}


