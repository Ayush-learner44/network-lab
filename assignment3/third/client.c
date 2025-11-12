#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

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

    // 🔑 Make both socket and stdin non‑blocking
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    printf("Connected to server. Start chatting...\n");

    while (1)
    {
        // 1️⃣ Try reading from stdin
        int n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = '\0';
            send(sockfd, buffer, n, 0);
        }

        // 2️⃣ Try receiving from server
        n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = '\0';
            printf("Server: %s", buffer);
            fflush(stdout);
        }

        usleep(100000); // small pause
    }

    close(sockfd);
    return 0;
}
