#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    char out_buf[strlen(argv[1]) + 7];
    char err_buf[strlen(argv[1]) + 7];
    for (int i = 0; i < sizeof(out_buf); i++) {
        out_buf[i] = 0;
        err_buf[i] = 0;
    }
    strcat(out_buf, argv[1]);
    strcat(err_buf, argv[1]);
    int out_fd = open(strcat(out_buf, ".stdout"), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int err_fd = open(strcat(err_buf, ".stderr"), O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fork() == 0) {
        close(1);
        close(2);
        dup(out_fd);
        dup(err_fd);
        close(out_fd);
        close(err_fd);
        execvp(argv[2], &argv[2]);
    }
}
