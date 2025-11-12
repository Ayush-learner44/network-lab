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
    char buf[200], input[100];

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    printf("Enter address OR name (Ctrl+D to quit):\n");
    while (fgets(input, sizeof(input), stdin))
    {
        int isnum = 1;
        for (int i = 0; input[i]; i++)
            if (input[i] == '\n')
                break;
            else if (input[i] < '0' || input[i] > '9')
            {
                isnum = 0;
                break;
            }

        // build request
        int pos = 0;
        if (isnum)
            buf[pos++] = 0x09; // number request
        else
            buf[pos++] = 0x01; // name request

        int len = strlen(input);
        if (input[len - 1] == '\n')
            input[len - 1] = 0;
        strcpy(&buf[pos], input);
        pos += strlen(input);
        buf[pos++] = 0x03; // ETX

        write(s, buf, pos);

        int n = read(s, buf, sizeof(buf) - 1);
        if (n <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }
        buf[n - 1] = 0; // remove ETX
        printf("Server replied: %s\n", buf);
    }

    close(s);
    return 0;
}
