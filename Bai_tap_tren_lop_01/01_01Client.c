#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZE];
    char host_name[128];
    char disks[10][128];
    int num_disks = 0;

    printf("Enter hostname: ");
    scanf("%s", host_name);
    printf("Enter number of disks: ");
    scanf("%d", &num_disks);

    for (int i = 0; i < num_disks; i++) {
        printf("Enter disk %d (letter and size, e.g., C-500GB): ", i + 1);
        scanf("%s", disks[i]);
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Initialize server address and connect to server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    // Send data to server
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s\n%d\n", host_name, num_disks);

    for (int i = 0; i < num_disks; i++) {
        strcat(buf, disks[i]);
        strcat(buf, "\n");
    }

    send(sockfd, buf, strlen(buf), 0);

    printf("Data sent to server:\n%s", buf);

    close(sockfd);

    return 0;
}
