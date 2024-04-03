#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    fd_set readfds;
    struct timeval timeout;

    system("mkfifo transit");
    int transit = open("transit", O_RDWR);
    printf("%d\n", transit);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); // Monitor stdin
        FD_SET(transit, &readfds); // Monitor pipe

        timeout.tv_sec = 5; // 5 seconds timeout
        timeout.tv_usec = 0;

        int ready = select(transit + 1, &readfds, NULL, NULL, &timeout);
        if (ready == 0) {
            write(1, "tick\n", 5);
        } else {
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                break; // Exit the loop when input is received
            }
            if (FD_ISSET(transit, &readfds)) {
                char buffer[256];
                ssize_t nbytes = read(transit, buffer, sizeof(buffer));
                write(1, buffer, nbytes);
                break;
            }
        }
    }

    close(transit);

    return 0;
}
