#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 6) {
        printf("Usage: %s <file_name> <receiver_ip_address> <receiver_port_number> <sender_port_number> <sender_id>\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];
    char *receiver_ip_address = argv[2];
    int receiver_port_number = atoi(argv[3]);
    int sender_port_number = atoi(argv[4]);
    int sender_id = atoi(argv[5]);

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Error: File not found\n");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        printf("Error: Failed to create socket\n");
        return 1;
    }

    struct sockaddr_in receiver_addr, sender_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    memset(&sender_addr, 0, sizeof(sender_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = inet_addr(receiver_ip_address);
    receiver_addr.sin_port = htons(receiver_port_number);
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sender_addr.sin_port = htons(sender_port_number);

    if (bind(sock, (struct sockaddr *)&sender_addr, sizeof(sender_addr)) == -1) {
        printf("Error: Failed to bind socket\n");
        close(sock);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    int bytes_sent, total_bytes_sent = 0;
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = fread(buffer, 1, sizeof(buffer), fp);
        if (bytes_read == 0) {
            break;
        }
        bytes_sent = sendto(sock, buffer, bytes_read, 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));
        if (bytes_sent == -1) {
            printf("Error: Failed to send data\n");
            break;
        }
        total_bytes_sent += bytes_sent;
        printf("Sender %d: %d bytes sent\n", sender_id, total_bytes_sent);
        sleep(1); // tạm dừng chương trình 1 giây
    }

    printf("Sender %d: Total of %d bytes sent\n", sender_id, total_bytes_sent);

    fclose(fp);
    close(sock);

    return 0;
}