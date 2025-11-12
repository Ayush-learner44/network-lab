#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];
    fd_set readfds;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Connected to server. Type messages...\n");

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);      // stdin
        FD_SET(sockfd, &readfds); // socket

        int maxfd = sockfd > 0 ? sockfd : 0;

        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &readfds)) // keyboard input
        {
            fgets(buffer, sizeof(buffer), stdin);
            send(sockfd, buffer, strlen(buffer) + 1, 0);
        }

        if (FD_ISSET(sockfd, &readfds)) // incoming message
        {
            int n = recv(sockfd, buffer, sizeof(buffer), 0);
            if (n <= 0)
            {
                printf("Server disconnected.\n");
                break;
            }
            printf("Server: %s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}
