#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "dict.h"
#include "parser.h"

typedef struct {
    char *buffer;
    size_t bufferSize;
    size_t bufferUsed;
} client_t;

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(6379);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        return 1;
    }

    listen(server_fd, 10);

    socklen_t client_len = sizeof(struct sockaddr_in);
    struct sockaddr_in clientAddr;

    dict *dictionary = dictCreate(4096);

    struct pollfd fds[1024] = {0};
    client_t clients[1024] = {0};
    int nfds = 1;
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (true) {
        if (poll(fds, nfds, -1) < 0) {
            perror("poll failed");
            continue;
        }

        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                if (i == 0) {
                    int client_fd = accept(server_fd, (struct sockaddr*)&clientAddr, &client_len);
                    if (client_fd < 0) perror("accept failed");
                    else {
                        clients[nfds].buffer = calloc(1024, 1);
                        clients[nfds].bufferSize = 1024;
                        clients[nfds].bufferUsed = 0;
                        fds[nfds].fd = client_fd;
                        fds[nfds].events = POLLIN;
                        nfds++;
                    }
                } else {
                    client_t *c = &clients[i];
                    if (c->bufferSize <= c->bufferUsed) {
                        c->bufferSize *= 2;
                        c->buffer = realloc(c->buffer, c->bufferSize);
                        if (!c->buffer) exit(1);
                    }

                    int n = recv(fds[i].fd, c->buffer + c->bufferUsed,
                                 c->bufferSize - c->bufferUsed, 0);
                    if (n <= 0) {
                        free(c->buffer);
                        close(fds[i].fd);
                        clients[i] = clients[nfds - 1];
                        fds[i] = fds[nfds - 1];
                        nfds--;
                        continue;
                    }
                    c->bufferUsed += n;

                    size_t byteLen;
                    int arraySize = respRequestArrayLen(c->buffer, c->bufferUsed, &byteLen);

                    if (arraySize > 0) {
                        respStr *args = getArgsFromRespArray(c->buffer, byteLen, arraySize);

                        if (arraySize >= 3 && respStrCmpWithCstr(&args[0], "SET\0")) {
                            dictSet(dictionary, &args[1], &args[2]);
                            send(fds[i].fd, "+OK\r\n", 5, 0);
                        } else if (arraySize >= 2 && respStrCmpWithCstr(&args[0], "GET\0")) {
                            respStr *value = dictGet(dictionary, &args[1]);
                            if (value) {
                                send(fds[i].fd, "+", 1, 0);
                                send(fds[i].fd, value->str, value->size, 0);
                                send(fds[i].fd, "\r\n", 2, 0);
                            } else send(fds[i].fd, "-ERR\r\n", 6, 0);
                        } else if (arraySize >= 2 && respStrCmpWithCstr(&args[0], "DEL\0")) {
                            dictDelete(dictionary, &args[1]);
                            send(fds[i].fd, "+OK\r\n", 5, 0);
                        }
                        freeRespArgs(args, arraySize);
                        memmove(c->buffer, c->buffer + byteLen, c->bufferUsed - byteLen);
                        c->bufferUsed -= byteLen;
                    } else if (arraySize == -1) {
                        c->bufferUsed = clearBufferFromInvalidRespRequest(c->buffer, c->bufferUsed);
                        send(fds[i].fd, "-ERR\r\n", 6, 0);
                        perror("invalid RESP syntax request received");
                    } else {
                        send(fds[i].fd, "-ERR hello how are you\r\n", 24, 0);
                        perror("invalid RESP request received");
                        continue;
                    }
                }
            }

            if (fds[i].revents & POLLHUP) {
                free(clients[i].buffer);
                close(fds[i].fd);
                clients[i] = clients[nfds - 1];
                fds[i] = fds[nfds - 1];
                nfds--;
            }
        }
    }

    dictFree(dictionary);
    close(server_fd);
    return 0;
}
