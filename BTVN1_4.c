#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define BUF_SIZE 1024

void log_client_info(struct sockaddr_in* client_addr, char* data);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port> <log_file>\n", argv[0]);
        return 1;
    }

    // Create socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket < 0) {
        perror("socket() failed");
        return 1;
    }

    // Bind to port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        return 1;
    }

    // Create log file
    FILE* log_file = fopen(argv[2], "a");
    if (log_file == NULL) {
        perror("fopen() failed");
        return 1;
    }

    printf("Server is listening on port %s...\n", argv[1]);

    // Receive data
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buf[BUF_SIZE];
    while (1) {
        memset(buf, 0, BUF_SIZE);
        ssize_t num_bytes = recvfrom(server_socket, buf, BUF_SIZE, 0,
                                     (struct sockaddr*) &client_addr, &client_addr_len);
        if (num_bytes < 0) {
            perror("recvfrom() failed");
            return 1;
        }

        // Print and log client info
        log_client_info(&client_addr, buf);

        // Write client data to log file
        time_t now = time(NULL);
        struct tm* time_info = localtime(&now);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        fprintf(log_file, "%s %s %s\n", inet_ntoa(client_addr.sin_addr), time_str, buf);
    }

    // Close socket and log file
    fclose(log_file);
    close(server_socket);

    return 0;
}

void log_client_info(struct sockaddr_in* client_addr, char* data) {
    printf("Received data from %s:%d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
    printf("Data: %s\n", data);
}
