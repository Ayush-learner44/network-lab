#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main()
{
    int sockfd, newsock;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[1024];
    socklen_t len = sizeof(cliaddr);
    fd_set readfds;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);
    printf("Server waiting...\n");

    newsock = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
    printf("Client connected. Start chatting...\n");

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);       // stdin
        FD_SET(newsock, &readfds); // client socket

        int maxfd = newsock > 0 ? newsock : 0;

        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &readfds)) // keyboard input
        {
            fgets(buffer, sizeof(buffer), stdin);
            send(newsock, buffer, strlen(buffer) + 1, 0);
        }

        if (FD_ISSET(newsock, &readfds)) // incoming message
        {
            int n = recv(newsock, buffer, sizeof(buffer), 0);
            if (n <= 0)
            {
                printf("Client disconnected.\n");
                break;
            }
            printf("Client: %s\n", buffer);
        }
    }

    close(newsock);
    close(sockfd);
    return 0;
}
