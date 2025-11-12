#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

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

    // 🔑 Make both socket and stdin non‑blocking
    fcntl(newsock, F_SETFL, O_NONBLOCK);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    while (1)
    {
        // 1️⃣ Try reading from stdin
        int n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = '\0';
            send(newsock, buffer, n, 0);
        }

        // 2️⃣ Try receiving from client
        n = recv(newsock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = '\0';
            printf("Client: %s", buffer);
            fflush(stdout);
        }

        usleep(100000); // small pause
    }

    close(newsock);
    close(sockfd);
    return 0;
}
