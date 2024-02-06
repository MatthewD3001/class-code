#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_READ 50

int main(){
	int fd = open("./read.c", O_RDWR|O_CREAT, 0644);
	int fdW = open("./output.c", O_RDWR|O_CREAT, 0644);
	char buf[MAX_READ] = {0};
	int size = 0;
	int cycles = 0;
	do {
		size = read(fd, buf, MAX_READ);
		write(fdW, buf, size);
		cycles++;
	}
	while(size);
	printf("Number of cycles ran: %d\n", cycles);
	return 0;
}
