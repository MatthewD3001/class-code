#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#define BUFLEN 128
#define NOT_CONNECTED 0
#define CONNECTED 1
#define LISTING 2

struct socket {
    int fd;
    int status;
};

void print_sockets(struct socket sockets[32]) {
    printf("[");
    for (int i = 0; i < 15; i++) {
        printf("(%d, %d), ", sockets[i].fd, sockets[i].status);
    }
    printf("(%d, %d),\n", sockets[15].fd, sockets[15].status);
    for (int i = 16; i < 31; i++) {
        printf("(%d, %d), ", sockets[i].fd, sockets[i].status);
    }
    printf("(%d, %d)]\n", sockets[31].fd, sockets[31].status);
}

void add_connection(int connection, struct socket sockets[32], int file_pos[32]) {
    // Find first available in array then insert data and set connected status, also init file position to zero
    for (int i = 0; i < 32; i++) {
        if (sockets[i].fd < 0) {
            sockets[i].fd = connection;
            sockets[i].status = CONNECTED;
            file_pos[i] = 0;
            return;
        }
    }
}

int main() {


    // Make the array for connections using the struct
    struct socket sockets[32];
    // Make struct for file positions during listing
    int file_pos[32]; 
    // Initialize all of the values to the desired ones
    for (int i = 0; i < 32; i++) {
        sockets[i].fd = -1;
        sockets[i].status = NOT_CONNECTED;
    }
    print_sockets(sockets);

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

    listen(server_socket, 32);

    
    fd_set readfds;

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        // Listening on this, always add to set
        FD_SET(server_socket, &readfds);

        // Listen on all active sockets
        for (int i = 0; i < 32; i++) {
            if (sockets[i].status) {
                FD_SET(sockets[i].fd, &readfds);
            } else {
                break;
            }
        }

        // 33 here to account for all possible connections made
        int status = select(server_socket + 33, &readfds, 0, 0, 0);
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
            if (FD_ISSET(server_socket, &readfds)) {
                struct sockaddr_in connect_addr = { 0 };
                int length_connect = sizeof(connect_addr);
                int connection = accept(server_socket, (struct sockaddr *) &connect_addr, (socklen_t *) &length_connect);
                printf("Accepted connection from address: %d on port: %d\n", ntohs(connect_addr.sin_addr.s_addr), ntohs(connect_addr.sin_port));
                add_connection(connection, sockets, file_pos);
                print_sockets(sockets);
                write(connection, "log #: ", sizeof("log #: "));
            }
            // Check all valid connections for input
            for (int i = 0; i < 32; i++) {
                if (sockets[i].status == CONNECTED) {
                    int conn_fd = sockets[i].fd;
                    if (FD_ISSET(conn_fd, &readfds)) {
                        readlen = read(conn_fd, buf, BUFLEN);
                        write(1, buf, readlen);
                        write(conn_fd, "log #: ", sizeof("log #: "));
                    }
                } else {
                    break;
                }
            }
        }
    }
}
