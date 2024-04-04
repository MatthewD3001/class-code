#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int status;
    char buf[256] = {0};
    int read_count = 0;
    struct timeval delay;
    for (;;) {
        fd_set in_fd;
        FD_ZERO(&in_fd);
        FD_SET(0, &in_fd);
        delay.tv_sec = 3;
        delay.tv_usec = 0;
        status = select(1, &in_fd, NULL, NULL, &delay);
        if (status < 0) {
            printf("Select error!\n");
            exit(0);
        }
        if (status == 0) {
            printf("tick\n");
        }
        if (status > 0 && FD_ISSET(0, &in_fd)) {
            read_count = read(0, buf, 256);
            write(1, buf, read_count);
        }
    }
}
