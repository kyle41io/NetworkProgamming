#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 256

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, n;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 4) {
        fprintf(stderr,"Usage: %s <port> <greeting_file> <output_file>\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0)
        error("ERROR on accept");

    int greeting_file = open(argv[2], O_RDONLY);
    if (greeting_file < 0)
        error("ERROR opening greeting file");

    char greeting_buffer[BUFFER_SIZE];
    bzero(greeting_buffer, BUFFER_SIZE);
    n = read(greeting_file, greeting_buffer, BUFFER_SIZE - 1);
    if (n < 0)
        error("ERROR reading greeting file");
    printf("Sending greeting message: %s\n", greeting_buffer);

    n = write(newsockfd, greeting_buffer, strlen(greeting_buffer));
    if (n < 0)
        error("ERROR writing to socket");

    int output_file = open(argv[3], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (output_file < 0)
        error("ERROR creating output file");

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0)
            error("ERROR reading from socket");
        if (n == 0)
            break;

        printf("Received message: %s", buffer);

        n = write(output_file, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to output file");
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}
