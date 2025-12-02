#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <service_name>\n", argv[0]);
        return 1;
    }

    int s, c;
    struct sockaddr_in addr, client_addr;
    socklen_t client_len;
    char buf[1024];
    int client_count = 0;

    struct servent *serv = getservbyname(argv[1], "tcp");
    if (!serv)
    {
        perror("getservbyname");
        return 1;
    }

    signal(SIGCHLD, SIG_IGN);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = serv->s_port;
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    listen(s, 5);
    printf("Server ready on service %s (port %d)...\n", argv[1], ntohs(serv->s_port));

    while (1)
    {
        client_len = sizeof(client_addr);

        c = accept(s, (struct sockaddr *)&client_addr, &client_len);
        if (c < 0)
            continue;

        struct hostent *h = gethostbyaddr(&client_addr.sin_addr, sizeof(client_addr.sin_addr), AF_INET);

        if (h)
            printf("Request received from %s\n", h->h_name);
        else
            printf("Request received from %s\n", inet_ntoa(client_addr.sin_addr));

        client_count++;
        printf("✅ Client %d connected (Total: %d)\n", client_count, client_count);

        if (fork() == 0)
        {
            close(s);

            int n = read(c, buf, sizeof(buf) - 1);
            if (n > 0)
            {
                buf[n] = '\0';
                int fd = open(buf, O_RDONLY);
                if (fd >= 0)
                {
                    while ((n = read(fd, buf, sizeof(buf))) > 0)
                        write(c, buf, n);

                    close(fd);
                }
                else
                {
                    char *msg = "File not found or cannot open.\n";
                    write(c, msg, strlen(msg));
                }
            }

            close(c);
            exit(0);
        }
        else
        {
            close(c);
        }
    }
}
