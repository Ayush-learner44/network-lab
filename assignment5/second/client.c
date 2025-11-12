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
    if (argc != 4)
    {
        printf("Usage: %s <server-ip> <port> <file_name>\n", argv[0]);
        return 1;
    }

    int s;
    struct sockaddr_in addr;
    char buf[1024];

    // create socket
    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    // send requested filename
    write(s, argv[3], strlen(argv[3]));

    // open local file to save received data
    int fd = creat("receive.txt", 0666);
    if (fd < 0)
    {
        perror("creat");
        close(s);
        return 1;
    }

    // receive file data
    int n;
    while ((n = read(s, buf, sizeof(buf))) > 0)
    {
        write(fd, buf, n);
    }

    printf("File received and saved as receive.txt\n");

    close(fd);
    close(s);
    return 0;
}
