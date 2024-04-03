#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int status;
    char buf[256] = {0};
    int read_count = 0;
    struct timeval delay;
    int pipe_fd = open("transit", O_RDWR);
    for (;;) {
        fd_set in_fd;
        FD_ZERO(&in_fd);
        FD_SET(0, &in_fd);
        FD_SET(pipe_fd, &in_fd);
        delay.tv_sec = 3;
        delay.tv_usec = 0;
        status = select(pipe_fd+1, &in_fd, NULL, NULL, &delay);
        if (status < 0) {
            write(1, "Select error!\n", 14);
            exit(0);
        }
        if (status == 0) {
            write(1, "tick\n", 5);
            printf("tick\n");
        }
        if (status > 0 && FD_ISSET(0, &in_fd)) {
            read_count = read(0, buf, 256);
            write(1, buf, read_count);
        }
        if (status > 0 && FD_ISSET(pipe_fd, &in_fd)) {
            read_count = read(pipe_fd, buf, 256);
            write(1, buf, read_count);
        }
    }
}
