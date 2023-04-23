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

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
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

    printf("Client connected.\n");

    bzero(buffer, BUFFER_SIZE);
    n = read(newsockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0)
        error("ERROR reading from socket");

    printf("Received data: %s", buffer);

    // Split data into lines
    char *lines[100];
    int num_lines = 0;
    char *token = strtok(buffer, "\n");
    while (token != NULL) {
        lines[num_lines++] = token;
        token = strtok(NULL, "\n");
    }

    // Write data to output file
    int output_file = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (output_file < 0)
        error("ERROR creating output file");

    for (int i = 1; i < num_lines; i++) {
        char *line = lines[i];
        char *drive = strtok(line, "-");
        char *size = strtok(NULL, "-");
        if (drive == NULL || size == NULL)
            continue;
        printf("%s: %s\n", drive, size);
        char output_line[BUFFER_SIZE];
        snprintf(output_line, BUFFER_SIZE, "%s: %s\n", drive, size);
        n = write(output_file, output_line, strlen(output_line));
        if (n < 0)
            error("ERROR writing to output file");
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}
