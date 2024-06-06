#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>

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
    accept(server_socket, (struct sockaddr *) &connect_addr, (socklen_t *) &length_connect);

    printf("Accepted connection from address: %d on port: %d\n", ntohs(connect_addr.sin_addr.s_addr), ntohs(connect_addr.sin_port));

}
