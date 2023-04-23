#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <receiver_port_number> <file_name>\n", argv[0]);
        return 1;
    }

    int receiver_port_number = atoi(argv[1]);
    char *file_name = argv[2];

    FILE *fp = fopen(file_name, "w");
    if (fp == NULL) {
        printf("Error: Failed to create file\n");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        printf("Error: Failed to create socket\n");
        return 1;
    }

    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    receiver_addr.sin_port = htons(receiver_port_number);

    if (bind(sock, (struct sockaddr*)&receiver_addr, sizeof(receiver_addr)) == -1) {
        printf("Error: Failed to bind socket\n");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    int seq_number = 0;
    int bytes_received;
    int file_complete = 0;

    while (!file_complete) {
        struct sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);
        bytes_received = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
        if (bytes_received < 0) {
            printf("Error: Failed to receive data\n");
            return 1;
        }
        int received_seq_number;
        memcpy(&received_seq_number, buffer, sizeof(received_seq_number));
        if (received_seq_number == seq_number) {
            fwrite(buffer + 4, 1, bytes_received - 4, fp);
            seq_number++;
        }
        if (bytes_received < BUFFER_SIZE + 4) {
            file_complete = 1;
        }
    }

    fclose(fp);
    close(sock);

    return 0;
}