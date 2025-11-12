#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sockfd, newsock;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[1024];
    socklen_t len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);
    printf("Server waiting...\n");

    newsock = accept(sockfd, (struct sockaddr *)&cliaddr, &len);

    for (int i = 0; i < 6; i++)
    {
        recv(newsock, buffer, sizeof(buffer), 0);
        printf("Client: %s\n", buffer);
        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);
        send(newsock, buffer, strlen(buffer) + 1, 0);
    }

    close(newsock);
    close(sockfd);
    return 0;
}