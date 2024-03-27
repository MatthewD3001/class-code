#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    printf("argv index 1: %s", argv[1]);
    execvp(argv[1], argv++);
}
