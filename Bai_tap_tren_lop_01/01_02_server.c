#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <port_number>\n", argv[0]);
        return 1;
    }

    int port_number = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error: Could not create socket");
        return 1;
    }

    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error: Could not bind socket");
        return 1;
    }

    listen(sockfd, 5);

    int client_sockfd;
    socklen_t client_address_len = sizeof(client_address);

    while (1) {
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_address_len);

        if (client_sockfd < 0) {
            perror("Error: Could not accept client connection");
            return 1;
        }

        char buffer[BUFFER_SIZE];
        int count = 0;

        while (recv(client_sockfd, buffer, BUFFER_SIZE, 0)) {
            if (strstr(buffer, "0123456789")) {
                count++;
            }
            memset(buffer, 0, BUFFER_SIZE);
        }

        printf("Number of occurrences: %d\n", count);

        close(client_sockfd);
    }

    close(sockfd);

    return 0;
}