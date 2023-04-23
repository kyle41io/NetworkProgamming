#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <server_ip_address> <port_number>\n", argv[0]);
        return 1;
    }

    char *server_ip_address = argv[1];
    int port_number = atoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error: Could not create socket");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    inet_pton(AF_INET, server_ip_address, &server_address.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error: Could not connect to server");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    FILE *file = fopen("data.txt", "r");
    if (!file) {
        perror("Error: Could not open file");
        return 1;
    }

    while (fgets(buffer, BUFFER_SIZE, file)) {
        send(sockfd, buffer, strlen(buffer), 0);
    }

    fclose(file);
    close(sockfd);

    return 0;
}