#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

typedef struct packet {
    char bSOF[5];
    char bSrcAddress[16];
    char bDstAddress[16];
    char bPayload[100];
} packet;

void *server_handler(void *fd_pointer);

int main() {
    int listenfd, connfd, *new_sock;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd == -1) {
        perror("Could not create Socket \n");
    }

    puts("Socket Created");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8888);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return 1;
    }

    puts("bind success");
    listen(listenfd, 5);

    puts("Waiting for connections");
    clilen = sizeof(cliaddr);

    while ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen))) {
        puts("Connection accepted");
        pthread_t server_thread;
        new_sock = malloc(1);
        *new_sock = connfd;
        pthread_create(&server_thread, NULL, server_handler, (void *)new_sock);
    }

    if (connfd < 0) {
        perror("Accecpt Failed");
        return 1;
    }
    return 0;
}

void *server_handler(void *fd_pointer) {
    int sock = *(int *)fd_pointer;
    int read_size, write_size;
    struct packet pack;

    struct packet packSend;
    
    while ((read_size = recv(sock, (char *)&pack, sizeof(pack), 0)) > 0) {
        printf("%s", pack.bPayload);
        char foo[80] = "test";

        strcpy(packSend.bSOF, "0xAA");
        strcpy(packSend.bSrcAddress, "127.0.0.1");
        strcpy(packSend.bDstAddress, "127.0.0.1");
        strcpy(packSend.bPayload, "This is the server response");

        if(write(sock, (char *)&packSend, sizeof(packSend)) <= 0) {
            printf("%s", "Error writing data to incoming socket");
        }
        fflush(stdin);
        fflush(stdout);
    }
    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (read_size == -1) {
        perror("recv failed");
    }

    return 0;
}
