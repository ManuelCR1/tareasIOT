#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

void *server_handler(void *fd_pointer);

void accelerometer(int opt, int sock, char *json_string, char *client_message);
void gyroscope(int opt, int sock, char *json_string, char *client_message);
void magnetometer(int opt, int sock, char *json_string, char *client_message);
void sendAll(int opt, int sock, char *json_string, char *client_message);

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
    char *message;
    char client_message[100];
    

    while ((read_size = recv(sock, client_message, 100, 0)) > 0) {
        FILE *fp;
        long file_size;

        char *json_string = "";
        fp = fopen("./data.json", "rb");
        if (fp == NULL) {
            printf("Error opening file.\n");
        }

        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        json_string = (char*) malloc(file_size + 1);
        fread(json_string, 1, file_size, fp);
        json_string[file_size] = '\0';

        printf("********** %s", client_message);

        fclose(fp);


        if (!strncmp("0xAA", client_message, 4)) {
            if (!strncmp("0xAA01", client_message, 6)) {
                if (!strncmp("0xAA0101", client_message, 8)) {
                    accelerometer(1, sock, json_string, client_message);
                } else if (!strncmp("0xAA0102", client_message, 8)) {
                    accelerometer(2, sock, json_string, client_message);
                } else if (!strncmp("0xAA0103", client_message, 8)) {
                    accelerometer(3, sock, json_string, client_message);
                } else if (!strncmp("0xAA0104", client_message, 8)) {
                    accelerometer(4, sock, json_string, client_message);
                } else {
                    char buffer[100];
                    strcpy(buffer, "Missing axis argument \n");
                    if(write(sock, buffer, strlen(buffer)) <= 0) {
                        printf("%s", "Error writing data to incoming socket");
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
            } else if (!strncmp("0xAA02", client_message, 6)) {
                if (!strncmp("0xAA0201", client_message, 8)) {
                    magnetometer(1, sock, json_string, client_message);
                } else if (!strncmp("0xAA0202", client_message, 8)) {
                    magnetometer(2, sock, json_string, client_message);
                } else if (!strncmp("0xAA0203", client_message, 8)) {
                    magnetometer(3, sock, json_string, client_message);
                } else if (!strncmp("0xAA0204", client_message, 8)) {
                    magnetometer(4, sock, json_string, client_message);
                } else {
                    char buffer[100];
                    strcpy(buffer, "Missing axis argument \n");
                    if(write(sock, buffer, strlen(buffer)) <= 0) {
                        printf("%s", "Error writing data to incoming socket");
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
            } else if (!strncmp("0xAA03", client_message, 6)) {
                if (!strncmp("0xAA0301", client_message, 8)) {
                    gyroscope(1, sock, json_string, client_message);
                } else if (!strncmp("0xAA0302", client_message, 8)) {
                    gyroscope(2, sock, json_string, client_message);
                } else if (!strncmp("0xAA0303", client_message, 8)) {
                    gyroscope(3, sock, json_string, client_message);
                } else if (!strncmp("0xAA0304", client_message, 8)) {
                    gyroscope(4, sock, json_string, client_message);
                } else {
                    char buffer[100];
                    strcpy(buffer, "Missing axis argument \n");
                    if(write(sock, buffer, strlen(buffer)) <= 0) {
                        printf("%s", "Error writing data to incoming socket");
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
            } else if (!strncmp("0xAAFF", client_message, 6)) {
                if (!strncmp("0xAAFF01", client_message, 8)) {
                    sendAll(1, sock, json_string, client_message);
                } else if (!strncmp("0xAAFF02", client_message, 8)) {
                    sendAll(2, sock, json_string, client_message);
                } else if (!strncmp("0xAAFF03", client_message, 8)) {
                    sendAll(3, sock, json_string, client_message);
                } else if (!strncmp("0xAAFF04", client_message, 8)) {
                    sendAll(4, sock, json_string, client_message);
                } else {
                    char buffer[100];
                    strcpy(buffer, "Missing axis argument \n");
                    if(write(sock, buffer, strlen(buffer)) <= 0) {
                        printf("%s", "Error writing data to incoming socket");
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
            } else {
                char buffer[100];
                strcpy(buffer, "Missing sensor argument \n");
                if(write(sock, buffer, strlen(buffer)) <= 0) {
                    printf("%s", "Error writing data to incoming socket");
                }
                memset(buffer, 0, sizeof(buffer));
            }
        } else {
            char buffer[100];
            strcpy(buffer, "Bad preamble \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
            memset(buffer, 0, sizeof(buffer));
        }
        memset(client_message, 0, sizeof(client_message));
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

void accelerometer(int opt, int sock, char *json_string, char *client_message) {
    char buffer[1000];
    struct json_object *root = json_tokener_parse(json_string);
    if (root == NULL) {
        printf("Error parsing JSON.\n");
    }
    struct json_object *value;
    if (json_object_object_get_ex(root, client_message, &value)) {
        if (json_object_is_type(value, json_type_string)) {
            const char *str = json_object_get_string(value);
            char c[strlen(str) + 1];
            char foo[80];
            strcpy(c, str);
            strcat(foo, c);
            if(write(sock, foo, strlen(foo)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        }
    }
    json_object_put(root);               //DELETE ALL THE SWITCHES!
    switch (opt) {
        case 1:
            strcpy(buffer, " Acelerometro en X \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 2:
            strcpy(buffer, " Acelerometro en Y \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 3:
            strcpy(buffer, " Acelerometro en Z \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 4:
            strcpy(buffer, " Acelerometro en todos los ejes \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
    }
    memset(buffer, 0, sizeof(buffer));
    fflush(stdin);
    fflush(stdout);
}

void magnetometer(int opt, int sock, char *json_string, char *client_message) {
    char buffer[1000];
    struct json_object *root = json_tokener_parse(json_string);
    if (root == NULL) {
        printf("Error parsing JSON.\n");
    }
    struct json_object *value;
    if (json_object_object_get_ex(root, client_message, &value)) {
        if (json_object_is_type(value, json_type_string)) {
            const char *str = json_object_get_string(value);
            char c[strlen(str) + 1];
            char foo[80];
            strcpy(c, str);
            strcat(foo, c);
            if(write(sock, foo, strlen(foo)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        }
    }
    json_object_put(root);
    switch (opt) {
        case 1:
            strcpy(buffer, " Magnetometro en X \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 2:
            strcpy(buffer, " Magnetometro en Y \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 3:
            strcpy(buffer, " Magnetometro en Z \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 4:
            strcpy(buffer, " Magnetometro en todos los ejes \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
    }
    memset(buffer, 0, sizeof(buffer));
}

void gyroscope(int opt, int sock, char *json_string, char *client_message) {
    char buffer[1000];
    struct json_object *root = json_tokener_parse(json_string);
    if (root == NULL) {
        printf("Error parsing JSON.\n");
    }
    struct json_object *value;
    if (json_object_object_get_ex(root, client_message, &value)) {
        if (json_object_is_type(value, json_type_string)) {
            const char *str = json_object_get_string(value);
            char c[strlen(str) + 1];
            char foo[80];
            strcpy(c, str);
            strcat(foo, c);
            if(write(sock, foo, strlen(foo)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        }
    }
    json_object_put(root);
    switch (opt) {
        case 1:
            strcpy(buffer, " Giroscopio en X \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 2:
            strcpy(buffer, " Giroscopio en Y \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 3:
            strcpy(buffer, " Giroscopio en Z \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
        case 4:
            strcpy(buffer, " Giroscopio en todos los ejes \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
        break;
    }
    memset(buffer, 0, sizeof(buffer));
}

void sendAll(int opt, int sock, char *json_string, char *client_message) {
    char buffer[100];
    switch (opt) {
        case 1:
            accelerometer(4, sock, json_string, client_message);
            strcpy(buffer, "All sensors test \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
            memset(buffer, 0, sizeof(buffer));
        break;
        case 2:
            magnetometer(4, sock, json_string, client_message);
            strcpy(buffer, "All sensors test \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
            memset(buffer, 0, sizeof(buffer));
        break;
        case 3:
            gyroscope(4, sock, json_string, client_message);
            strcpy(buffer, "All sensors test \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
            memset(buffer, 0, sizeof(buffer));
        break;
        case 4:
            accelerometer(4, sock, json_string, client_message);
            magnetometer(4, sock, json_string, client_message);
            gyroscope(4, sock, json_string, client_message);
            strcpy(buffer, "All sensors test \n");
            if(write(sock, buffer, strlen(buffer)) <= 0) {
                printf("%s", "Error writing data to incoming socket");
            }
            memset(buffer, 0, sizeof(buffer));
        break;
    }
}
