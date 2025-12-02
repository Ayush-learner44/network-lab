#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <server_name> <service_name> <file_name>\n", argv[0]);
        return 1;
    }

    int s;
    struct sockaddr_in addr;
    char buf[1024];

    struct hostent *h = gethostbyname(argv[1]);
    if (!h)
    {
        perror("gethostbyname");
        return 1;
    }

    struct servent *serv = getservbyname(argv[2], "tcp");
    if (!serv)
    {
        perror("getservbyname");
        return 1;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = serv->s_port;
    memcpy(&addr.sin_addr, h->h_addr_list[0], h->h_length);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    write(s, argv[3], strlen(argv[3]));

    int fd = creat("receive.txt", 0666);
    if (fd < 0)
    {
        perror("creat");
        close(s);
        return 1;
    }

    int n;
    while ((n = read(s, buf, sizeof(buf))) > 0)
        write(fd, buf, n);

    printf("File received and saved as receive.txt\n");

    close(fd);
    close(s);
    return 0;
}
