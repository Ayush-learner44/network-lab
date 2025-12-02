#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h> // Required for gethostbyname

#define MAXLINE 4096
#define PORT 8080

int main(int argc, char *argv[])
{
    // The assignment requires: execute host command
    if (argc != 3)
    {
        printf("Usage: %s <server-host> <command>\n", argv[0]);
        printf("Example: %s localhost \"ls -l\"\n", argv[0]);
        return 1;
    }

    int s;
    struct sockaddr_in addr;
    struct hostent *server; // Structure for gethostbyname
    char buf[MAXLINE];
    socklen_t len;

    char *hostname = argv[1];
    char *command = argv[2];

    // 1. Create UDP Socket (DGRAM)
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        perror("socket");
        return 1;
    }

    // 2. Resolve Hostname (Assignment requirement)
    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr, "Error: No such host '%s'\n", hostname);
        return 1;
    }

    // 3. Setup Server Address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&addr.sin_addr.s_addr, server->h_length);
    addr.sin_port = htons(PORT);

    // 4. Send the Command
    // In UDP, we don't connect(), we just sendto()
    len = sizeof(addr);
    sendto(s, command, strlen(command), 0, (struct sockaddr *)&addr, len);

    // 5. Receive Output Loop
    // We set a timeout because UDP doesn't know when the server stops sending
    struct timeval tv;
    tv.tv_sec = 2; // Wait up to 2 seconds for data
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    printf("Command sent. Waiting for output...\n");
    printf("-----------------------------------\n");

    while (1)
    {
        int n = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&addr, &len);

        if (n < 0)
        {
            // Timeout reached, assume server is done
            break;
        }

        buf[n] = 0;
        printf("%s", buf);
    }
    printf("-----------------------------------\n");

    close(s);
    return 0;
}