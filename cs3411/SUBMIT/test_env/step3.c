#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#define BUFLEN 128

int main() {
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htonl(0)
    };

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    bind(server_socket, (struct sockaddr *) &addr, sizeof(addr));

    int length = sizeof(addr);
    getsockname(server_socket, (struct sockaddr *) &addr, (socklen_t *) &length);
    printf("Socket assigned to port: %d\n", ntohs(addr.sin_port));

    listen(server_socket, 1);

    struct sockaddr_in connect_addr = { 0 };
    int length_connect = sizeof(connect_addr);
    int connection = accept(server_socket, (struct sockaddr *) &connect_addr, (socklen_t *) &length_connect);

    printf("Accepted connection from address: %d on port: %d\n", ntohs(connect_addr.sin_addr.s_addr), ntohs(connect_addr.sin_port));
    
    fd_set readfds;

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(connection, &readfds);

        int status = select(connection + 1, &readfds, 0, 0, 0);
        if (status < 1) {
            write(2, "Select Error\n", sizeof("Select Error\n"));
            exit(EXIT_FAILURE);
        }
        if (status > 0) {
            char buf[BUFLEN] = {0};
            int readlen = 0;
            if (FD_ISSET(0, &readfds)) {
                readlen = read(0, buf, BUFLEN);
                write(1, buf, readlen);
            }
            if (FD_ISSET(connection, &readfds)) {
                readlen = read(connection, buf, BUFLEN);
                write(1, buf, readlen);
            }
        }
    }
}
