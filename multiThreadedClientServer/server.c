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
//void sendAll(int opt, int sock);

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
    char client_message[10];
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
    //json_string[file_size] = '\0';

    fclose(fp);
    

    while ((read_size = recv(sock, client_message, 10, 0)) > 0) {
        fflush(stdin);


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
                    char buffer[30];
                    strcpy(buffer, "Missing axis argument \n");
                    write(sock, buffer, strlen(buffer));
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
                    char buffer[30];
                    strcpy(buffer, "Missing axis argument \n");
                    write(sock, buffer, strlen(buffer));
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
                    char buffer[30];
                    strcpy(buffer, "Missing axis argument \n");
                    write(sock, buffer, strlen(buffer));
                }
            } else if (!strncmp("0xAAFF", client_message, 6)) {
                if (!strncmp("0xAAFF01", client_message, 8)) {
                    // sendAll(1, sock);
                } else if (!strncmp("0xAAFF02", client_message, 8)) {
                    // sendAll(2, sock);
                } else if (!strncmp("0xAAFF03", client_message, 8)) {
                    // sendAll(3, sock);
                } else if (!strncmp("0xAAFF04", client_message, 8)) {
                    // sendAll(4, sock);
                } else {
                    char buffer[30];
                    strcpy(buffer, "Missing axis argument \n");
                    write(sock, buffer, strlen(buffer));
                }
            } else {
                char buffer[30];
                strcpy(buffer, "Missing sensor argument \n");
                write(sock, buffer, strlen(buffer));
            }
        } else {
            char buffer[30];
            strcpy(buffer, "Bad preamble \n");
            write(sock, buffer, strlen(buffer));
        }
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
            write(sock, foo, strlen(foo));
        }
    }
    json_object_put(root);
    switch (opt) {
        case 1:
            strcpy(buffer, " Acelerometro en X \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 2:
            strcpy(buffer, " Acelerometro en X \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 3:
            strcpy(buffer, " Acelerometro en Z \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 4:
            strcpy(buffer, " Acelerometro en todos los ejes \n");
            write(sock, buffer, strlen(buffer));
        break;
    }
}

void magnetometer(int opt, int sock, char *json_string, char *client_message) {
    char buffer[100];
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
            write(sock, foo, strlen(foo));
        }
    }
    json_object_put(root);
    switch (opt) {
        case 1:
            strcpy(buffer, " Magnetometro en X \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 2:
            strcpy(buffer, " Magnetometro en Y \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 3:
            strcpy(buffer, " Magnetometro en Z \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 4:
            strcpy(buffer, " Magnetometro en todos los ejes \n");
            write(sock, buffer, strlen(buffer));
        break;
    }
}

void gyroscope(int opt, int sock, char *json_string, char *client_message) {
    char buffer[100];
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
            write(sock, foo, strlen(foo));
        }
    }
    json_object_put(root);
    switch (opt) {
        case 1:
            strcpy(buffer, " Giroscopio en X \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 2:
            strcpy(buffer, " Giroscopio en Y \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 3:
            strcpy(buffer, " Giroscopio en Z \n");
            write(sock, buffer, strlen(buffer));
        break;
        case 4:
            strcpy(buffer, " Giroscopio en todos los ejes");
            write(sock, buffer, strlen(buffer));
        break;
    }
}

// void sendAll(int opt, int sock) {
//     char buffer[100];
//     switch (opt) {
//         case 1:
//             accelerometer(1, sock);
//             magnetometer(1, sock);
//             gyroscope(1, sock);
//         break;
//         case 2:
//             accelerometer(2, sock);
//             magnetometer(2, sock);
//             gyroscope(2, sock);
//         break;
//         case 3:
//             accelerometer(3, sock);
//             magnetometer(3, sock);
//             gyroscope(3, sock);
//         break;
//         case 4:
//             accelerometer(4, sock);
//             magnetometer(4, sock);
//             gyroscope(4, sock);
//         break;
//     }
// }

// struct json_object * find_something(struct json_object *jobj, const char *key) {
// 	struct json_object *tmp;

// 	json_object_object_get_ex(jobj, key, &tmp);

// 	return tmp;
// }