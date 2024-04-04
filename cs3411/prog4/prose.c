#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 256

int isPrintable(unsigned char c) {
    if (c > 31 && c < 127) {
        return 1;
    } else {
        return 0;
    }
}

void translate(int fd, int len, unsigned char buf[BUFLEN]) {
    char hex[5]; // 4 + 1 for null byte of sprintf
    for (int i = 0; i < len; i++) {
        if (isPrintable(buf[i])) {
            write(fd, &buf[i], 1);
        } else {
            sprintf(hex, "<%02x>", buf[i]);
            write(fd, hex, 4);
        }
    }
}

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
    close(pipe_out[1]);
    close(pipe_err[1]);

    char out_buf[strlen(argv[1]) + 7];
    char err_buf[strlen(argv[1]) + 7];
    out_buf[0] = 0;
    err_buf[0] = 0;
    strcat(out_buf, argv[1]);
    strcat(err_buf, argv[1]);
    int out_fd = 0;
    int err_fd = 0;

    int status;
    unsigned char buf[BUFLEN] = {0};
    int read_count = 0;

    fd_set in_fd;
    FD_ZERO(&in_fd);
    FD_SET(pipe_out[0], &in_fd);
    FD_SET(pipe_err[0], &in_fd);
    status = select(pipe_err[1]+1, &in_fd, NULL, NULL, NULL);
    if (status < 0) {
        write(1, "Select error!\n", 14);
        exit(0);
    }
    if (status > 0 && FD_ISSET(pipe_out[0], &in_fd)) {
        out_fd = open(strcat(out_buf, ".stdout"), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        while (1) {
            read_count = read(pipe_out[0], buf, BUFLEN);
            if (read_count <= 0) {
                break;
            }
            translate(1, read_count, buf);
            write(out_fd, buf, read_count);
        }
    }
    if (status > 0 && FD_ISSET(pipe_err[0], &in_fd)) {
        err_fd = open(strcat(err_buf, ".stderr"), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        while (1) {
            read_count = read(pipe_err[0], buf, BUFLEN);
            if (read_count <= 0) {
                break;
            }
            translate(2, read_count, buf);
            write(err_fd, buf, read_count);
        }
    }
    return 0;
}
