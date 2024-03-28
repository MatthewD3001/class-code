#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    int pipe_out[2];
    int pipe_err[2];
    pipe(pipe_out);
    pipe(pipe_err);

    /**
    * Child process!!!!!
    */
    if (fork() == 0) {
        close(1);
        close(2);
        dup(pipe_out[1]);
        dup(pipe_err[1]);
        close(pipe_out[1]);
        close(pipe_err[1]);
        execvp(argv[2], &argv[2]);
    }

    char out_buf[strlen(argv[1]) + 7];
    char err_buf[strlen(argv[1]) + 7];
    out_buf[0] = 0;
    err_buf[0] = 0;
    strcat(out_buf, argv[1]);
    strcat(err_buf, argv[1]);
    int out_fd = open(strcat(out_buf, ".stdout"), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int err_fd = open(strcat(err_buf, ".stderr"), O_WRONLY | O_CREAT | O_TRUNC, 0644);

    int status;
    char buf[256] = {0};
    int read_count = 0;
    struct timeval delay;

    for (;;) {
        fd_set in_fd;
        FD_ZERO(&in_fd);
        FD_SET(pipe_out[0], &in_fd);
        FD_SET(pipe_err[0], &in_fd);
        delay.tv_sec = 2;
        delay.tv_usec = 0;
        status = select(pipe_err[1]+1, &in_fd, NULL, NULL, &delay);
        if (status < 0) {
            printf("Select error!\n");
            break;
        }
        if (status == 0) {
            printf("timeout!\n");
            break;
            // Timeout happened, stop child process??????
        }
        if (status > 0 && FD_ISSET(pipe_out[0], &in_fd)) {
            read_count = read(pipe_out[0], buf, 256);
            write(out_fd, buf, read_count);
        }
        if (status > 0 && FD_ISSET(pipe_err[0], &in_fd)) {
            read_count = read(pipe_err[0], buf, 256);
            write(err_fd, buf, read_count);
        }
    }
    close(pipe_out[0]);
    close(pipe_err[0]);
}
