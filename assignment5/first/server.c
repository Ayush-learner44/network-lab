#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int s, c;
    struct sockaddr_in addr;
    char buf[1024];

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 5);

    printf("Server ready on port %s...\n", argv[1]);

    while (1)
    {
        c = accept(s, NULL, NULL);

        // read requested filename
        int n = read(c, buf, sizeof(buf) - 1);
        if (n <= 0)
        {
            close(c);
            continue;
        }
        buf[n] = '\0';

        // open file
        int fd = open(buf, O_RDONLY);
        if (fd < 0)
        {
            perror("open");
            close(c);
            continue;
        }

        // send file contents
        while ((n = read(fd, buf, sizeof(buf))) > 0)
        {
            write(c, buf, n);
        }

        close(fd);
        close(c);
    }
}
