#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	int fd = open("./example.txt", O_RDWR|O_CREAT, 0644);
	char buf[50] = {0};
	int size = 0;
	size = read(fd, buf, 50);
	printf("%sNumber of things stored: %d\n", buf, size);
	return 0;
}
