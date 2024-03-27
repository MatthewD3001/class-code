#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    char buf[strlen(argv[1]) + 7];
    for (int i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }
    strcat(buf, argv[1]);
    int out_fd = open(strcat(buf, ".stdout"), O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fork() == 0) {
        close(1);
        dup(out_fd);
        close(out_fd);
        argv++;
        execvp(*argv, argv);
    }
}
