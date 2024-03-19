#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[]){
	int fd;

	fd = open(argv[1], O_RDWR);
	unsigned char dict[15]; 
	if (fd < 0){
		printf("Error opening encoded file.");
		exit(-1);
	}

	read(fd, dict, sizeof(dict));

	int i;
	for (i = 0; i < 15; i ++)
		printf("%02x ",dict[i]);
		printf("\n");
		for (i = 0; i < 15; i ++)
			if ((dict[i] >= 32) & (dict[i] <=126))
				printf("%c  ",dict[i]);
			else
				printf("   ");
		printf("\n");
}
