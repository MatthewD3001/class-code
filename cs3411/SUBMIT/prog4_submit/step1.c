#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    close(1);
    char buf[strlen(argv[1]) + 7];
    buf[0] = 0;
    strcat(buf, argv[1]);
    open(strcat(buf, ".stdout"), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    for (int i = 2; i < argc; i++) {
        write(1, argv[i], strlen(argv[i]));
    }
}
