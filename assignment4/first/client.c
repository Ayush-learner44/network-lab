#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <server-ip>\n", argv[0]);
        return 1;
    }

    int s;
    struct sockaddr_in addr;
    char buf[100];

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("socket");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return 1;
    }

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    printf("Connected to server %s:8080\n", argv[1]);
    printf("Enter address OR name (Ctrl+D to quit):\n");

    while (fgets(buf, sizeof(buf), stdin))
    {
        send(s, buf, strlen(buf), 0);
        int n = recv(s, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }
        buf[n] = 0;
        printf("Server replied: %s", buf);
    }

    close(s);
    return 0;
}
