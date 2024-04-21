#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFLEN 128
#define NOT_CONNECTED 0
#define CONNECTED 1
#define LISTING 2

struct socket {
    int fd;
    int status;
    int file_pos;
};


void add_connection(int connection, struct socket sockets[32]) {
    // Find first available in array then insert data and set connected status, also init file position to zero
    for (int i = 0; i < 32; i++) {
        if (sockets[i].fd < 0) {
            sockets[i].fd = connection;
            sockets[i].status = CONNECTED;
            sockets[i].file_pos = 0;
            return;
        }
    }
}

void list(struct socket sockets[32], int index, int log_file) {
    char log[BUFLEN] = { 0 };
    int conn_fd = sockets[index].fd;
    lseek(log_file, sockets[index].file_pos, SEEK_SET);
    int log_read = read(log_file, log, BUFLEN);
    write(conn_fd, log, log_read);
    if (log_read < BUFLEN) {
        write(conn_fd, "log #: ", strlen("log #: "));
        sockets[index].status = CONNECTED;
        sockets[index].file_pos = 0;
    } else {
        sockets[index].status = LISTING;
        sockets[index].file_pos = log_read;
    }
}

void parse_command(struct socket sockets[32], int index, int log_file, char buf[BUFLEN], int readlen) {
    char msg[BUFLEN] = { 0 };
    int conn_fd = sockets[index].fd;
    if (strncmp(buf, "log ", strlen("log ")) == 0) {
        sprintf(msg, "#log: logging\n");
        write(conn_fd, msg, strlen(msg));
        for (int i = 4; i < readlen - 2; i++) {
            write(log_file, &buf[i], 1);
        }
        write(log_file, "\n", 1);
        write(conn_fd, "log #: ", strlen("log #: "));
    } else if (strncmp(buf, "list\r\n", strlen("list\r\n")) == 0) {
        sprintf(msg, "#log: listing\n");
        write(conn_fd, msg, strlen(msg));
        list(sockets, index, log_file);
    } else if (strncmp(buf, "\r\n", strlen("\r\n")) == 0 && sockets[index].status == LISTING) {
        sprintf(msg, "#log: more\n");
        write(conn_fd, msg, strlen(msg));
        list(sockets, index, log_file);
    } else {
        sprintf(msg, "#log: Command not recognized\n");
        write(conn_fd, msg, strlen(msg));
        write(conn_fd, "log #: ", strlen("log #: "));
    }
}


int main() {

    // Make the array for connections using the struct
    struct socket sockets[32];
    // Make struct for file positions during listing
    // Initialize all of the values to the desired ones
    for (int i = 0; i < 32; i++) {
        sockets[i].fd = -1;
        sockets[i].status = NOT_CONNECTED;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htonl(0)
    };

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(server_socket, (struct sockaddr *) &addr, sizeof(addr))) {
        write(1, "Bind error\n", sizeof("Bind error\n"));
        return 1;
    }

    // report the port of the server and init a message buffer used to report to the user
    int length = sizeof(addr);
    getsockname(server_socket, (struct sockaddr *) &addr, (socklen_t *) &length);
    char msg[128] = { 0 };
    sprintf(msg, "Socket assigned to port: %d\n", ntohs(addr.sin_port));
    write(1, msg, strlen(msg));
    listen(server_socket, 32);

    int log_file = open("log_file", O_RDWR | O_TRUNC | O_CREAT | O_APPEND, 0644);

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
            // Check if any other connections are trying to be made
            if (FD_ISSET(server_socket, &readfds)) {
                struct sockaddr_in connect_addr = { 0 };
                int length_connect = sizeof(connect_addr);
                int connection = accept(server_socket, (struct sockaddr *) &connect_addr, (socklen_t *) &length_connect);
                sprintf(msg, "Accepted connection from address: %s on port: %d\n", inet_ntoa(connect_addr.sin_addr), ntohs(connect_addr.sin_port));
                write(1, msg, strlen(msg));
                add_connection(connection, sockets);
                write(connection, "log #: ", strlen("log #: "));
            }
            // Check all valid connections for input
            for (int i = 0; i < 32; i++) {
                if (sockets[i].status != NOT_CONNECTED) {
                    int conn_fd = sockets[i].fd;
                    if (FD_ISSET(conn_fd, &readfds)) {
                        readlen = read(conn_fd, buf, BUFLEN);
                        if (readlen == 0) {
                            close(sockets[i].fd);
                            sockets[i].fd = -1;
                            sockets[i].status = NOT_CONNECTED;
                            sockets[i].file_pos = 0;
                            break;
                        }
                        parse_command(sockets, i, log_file, buf, readlen);
                    }
                } else {
                    break;
                }
            }
        }
    }
}
