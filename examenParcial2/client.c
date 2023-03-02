#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct packet {
    char bSOF[5];
    char bSrcAddress[16];
    char bDstAddress[16];
    char bPayload[100];
};

int main() {
    int sockfd;
    ssize_t n;

    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        perror("Could not create socket");
    }

    printf("Created Socket \n");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != -1) {
        while (1) {
            struct packet pack;
            struct packet packReply;
            char server_reply[2000];

            strcpy(pack.bSOF, "0xAA");

            strcpy(pack.bSrcAddress, "127.0.0.1");
            strcpy(pack.bDstAddress, "127.0.0.1");

            printf("Payload:");
            fgets(pack.bPayload, sizeof(pack.bPayload), stdin);
            fflush(stdin);

            if (send(sockfd, (char *)&pack, sizeof(pack), 0) < 0) {
                printf("Error \n");
                return 1;
            }
            if ((n = recv(sockfd, server_reply, 2000, 0)) < 0) {
                puts("Error");
                break;
            } else if (n == 0) {
                puts("Connection closed by server");
                break;
            }
            printf("Server Reply: %s \n", server_reply);
            memset(server_reply, 0, sizeof(server_reply));
        }
    }
    return 0;
}
