#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Connected to server. Start chatting...\n");

    pid_t pid = fork();

    if (pid == 0)
    {
        // Child: send to server
        while (1)
        {
            if (fgets(buffer, sizeof(buffer), stdin) != NULL)
            {
                send(sockfd, buffer, strlen(buffer), 0);
            }
        }
    }
    else
    {
        // Parent: receive from server
        while (1)
        {
            int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0)
            {
                printf("Server disconnected.\n");
                break;
            }
            buffer[n] = '\0';
            printf("Server: %s", buffer);
            fflush(stdout);
        }
    }

    close(sockfd);
    return 0;
}
