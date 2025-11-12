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
    printf("Client connected. Start chatting...\n");

    pid_t pid = fork();

    if (pid == 0)
    {
        // Child: receive from client
        while (1)
        {
            int n = recv(newsock, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0)
            {
                printf("Client disconnected.\n");
                break;
            }
            buffer[n] = '\0';
            printf("Client: %s", buffer);
            fflush(stdout);
        }
    }
    else
    {
        // Parent: send to client
        while (1)
        {
            if (fgets(buffer, sizeof(buffer), stdin) != NULL)
            {
                send(newsock, buffer, strlen(buffer), 0);
            }
        }
    }

    close(newsock);
    close(sockfd);
    return 0;
}
