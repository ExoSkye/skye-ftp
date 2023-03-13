#include <log.h>

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>

#include <io/pad.h>
#include <ppu-types.h>
#include <sys/thread.h>

#include <string>

bool running = true;

void handle_client(void* clientfd_void) {
    intptr_t clientfd = (intptr_t)clientfd_void;

    int bufsize = 65536;

    char* buffer = (char*)malloc(bufsize);
    ssize_t bytes_read = 0;

    while (bytes_read = recv(clientfd, buffer, bufsize, 0)) {
        if (bytes_read < 0) {
            log_message("Error reading from client");
            break;
        }

        if (send(clientfd, buffer, bytes_read, 0) < 0) {
            log_message("Error writing to client");
            break;
        }

        //log_message("Client said: ");
        //log_message(buffer);
        //log_message("\n");
    }

    close(clientfd);
    free(buffer);
}

int main()
{
    log_message("App start\n");

    ioPadInit(7);
    padInfo info;

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in sa;
    sa.sin_addr.s_addr = INADDR_ANY;

    //int port = (int)(((float)rand() / (float)RAND_MAX) * 65535);
    int port = 1337;

    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;

    log_message("Running on 0.0.0.0:");
    log_message(std::to_string(port).c_str());
    log_message("\n");

	bind(sockfd, (const struct sockaddr *) &sa, sizeof(sa));

    bool running = true;

    size_t stack_size = 0x10000;
    char* thread_name = "client_thread";
    u64 thread_priority = 1500;

    sys_ppu_thread_t thread_id[16];
    u16 thread_count = 0;

    listen(sockfd, 10);

    while (running) {
        ioPadGetInfo(&info);

        if (info.status[0] & PAD_CTRL_START) {
            running = false;
        }

        int clientfd = accept(sockfd, NULL, NULL);

        if (clientfd < 0) {
            log_message("Error accepting client");
            continue;
        } else {
            log_message("Accepted client\n");
            sysThreadCreate(&thread_id[thread_count++], handle_client, (void*)clientfd, thread_priority, stack_size, 0, thread_name);
        }
    }

    close(sockfd);

	return 0;
}

