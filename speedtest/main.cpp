#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(argv[1], argv[2], &hints, &servinfo);

    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

    int i = 0;

    char buffer[65536];

    for (int i = 0; i < 65536; i++) {
        buffer[i] = (char)rand() % 256;
    }

    while (true) {

        send(sockfd, buffer, 65536, 0);
        recv(sockfd, buffer, 65536, 0);

        i++;

        printf("%d\n", i);
    }

    freeaddrinfo(servinfo); // free the linked-list
}