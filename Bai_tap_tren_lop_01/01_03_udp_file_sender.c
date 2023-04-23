#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <file_name> <receiver_ip_address> <receiver_port_number> <sender_port_number>\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];
    char *receiver_ip_address = argv[2];
    int receiver_port_number = atoi(argv[3]);
    int sender_port_number = atoi(argv[4]);

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

    struct sockaddr_in receiver_addr;
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = inet_addr(receiver_ip_address);
    receiver_addr.sin_port = htons(receiver_port_number);

    char buffer[BUFFER_SIZE];
    int seq_number = 0;
    int bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        char packet[BUFFER_SIZE + 4];
        memcpy(packet, &seq_number, sizeof(seq_number));
        memcpy(packet + 4, buffer, bytes_read);
        sendto(sock, packet, bytes_read + 4, 0, (struct sockaddr*)&receiver_addr, sizeof(receiver_addr));
        seq_number++;
    }

    fclose(fp);
    close(sock);

    return 0;
}